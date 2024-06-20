#include <future>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/WindowManager.hpp>
#include <Window/Window.hpp>

#include <Log/Wrapper.hpp>

static constexpr uint32_t NumberOfApps = 5;

using namespace Ame;

class MutliEngineSample : public IoCContainer
{
public:
    Co::result<void> Run()
    {
        Initialize();

        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        auto& rhiDevice   = GetSubsystem<Rhi::DeviceSubsystem>();

        auto thisExecutor = GetSubsystem<CoroutineSubsystem>().inline_executor();

        while (engineFrame.IsRunning())
        {
            auto frameTick   = thisExecutor->submit(std::bind(&EngineFrame::Tick, &engineFrame));
            auto frameRender = thisExecutor->submit(std::bind(&Rhi::Device::Tick, &rhiDevice));

            Co::when_all(thisExecutor, std::move(frameTick), std::move(frameRender)).run().wait();
        }

        co_return;
    }

protected:
    Co::null_result Initialize()
    {
        Log::Engine().Trace("Initializing Sample...");
        InitializeWindow(
            GetSubsystem<CoroutineSubsystem>(),
            GetSubsystem<Rhi::DeviceSubsystem>());

        UpdateOnFrames();

        co_return;
    }

private:
    Co::null_result UpdateOnFrames()
    {
        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        auto& rhiDevice   = GetSubsystem<Rhi::DeviceSubsystem>();

        Log::Client().Info("Waiting for engine frame...");

        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();
        auto& windowManager    = rhiDevice.GetWindowManager();

        while (engineFrame.IsRunning())
        {
            co_await Co::resume_on(engineFrame.GetStartFrameExecutor());
            co_await Co::resume_on(rhiDevice.GetExecutor(Rhi::ExecutorType::Graphics));

            auto submission = co_await commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);

            auto& backbuffer = (co_await windowManager.GetBackbuffer()).get();

            Rhi::TextureBarrierDesc backbufferTextureBarrier{
                .texture = backbuffer.Resource.Unwrap(),
                .before{ .access = Rhi::AccessBits::UNKNOWN, .layout = Rhi::LayoutType::UNKNOWN, .stages = Rhi::ShaderType::ALL },
                .after{ .access = Rhi::AccessBits::COLOR_ATTACHMENT, .layout = Rhi::LayoutType::COLOR_ATTACHMENT, .stages = Rhi::ShaderType::ALL }
            };
            Rhi::BarrierGroupDesc backbufferBarrier{
                .textures   = &backbufferTextureBarrier,
                .textureNum = 1
            };

            submission->ResourceBarrier(backbufferBarrier);

            {
                std::array renderTargets{ &backbuffer.View };
                std::array clearColors{ Rhi::ClearDesc::RenderTarget(Colors::c_GreenYellow) };

                Rhi::RenderingCommand rendering(submission.CommandListRef, renderTargets);
                submission->ClearAttachments(clearColors);
            }

            std::swap(backbufferTextureBarrier.after, backbufferTextureBarrier.before);
            backbufferTextureBarrier.after.layout = Rhi::LayoutType::PRESENT;

            submission->ResourceBarrier(backbufferBarrier);

            co_await commandSubmitter.SubmitCommandList(submission);
        }
    }

private:
    void InitializeWindow(
        Co::runtime& runtime,
        Rhi::Device& rhiDevice)
    {
        auto& windowManager = rhiDevice.GetWindowManager();
        auto& window        = windowManager.GetWindow();
        m_Title             = window.GetTitle();

        auto TimerQueue = runtime.timer_queue();
        m_TitleTimer    = TimerQueue->make_timer(
            std::chrono::seconds(1),
            std::chrono::seconds(1),
            runtime.thread_pool_executor(),
            [this]()
            {
                UpdateTitle(
                    GetSubsystem<FrameTimerSubsystem>(),
                    GetSubsystem<Rhi::DeviceSubsystem>());
            });
    }

    void UpdateTitle(
        FrameTimer&  timer,
        Rhi::Device& rhiDevice)
    {
        auto&  windowManager = rhiDevice.GetWindowManager();
        auto&  window        = windowManager.GetWindow();
        double fps           = 1.0 / timer.GetDeltaTime();
        window.SetTitle(std::format("{} - FPS: {:.2f}", m_Title, fps));
    }

private:
    String    m_Title;
    Co::timer m_TitleTimer;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");

    std::vector<std::future<void>> tasks;
    tasks.reserve(NumberOfApps);

    for (auto [backendType, rhiName] : {
             std::pair{ Rhi::DeviceType ::DirectX12, "DirectX 12" },
             std::pair{ Rhi::DeviceType::Vulkan, "Vulkan" },
         })
    {
        for (size_t i = 0; i < NumberOfApps; i++)
        {
            tasks.emplace_back(
                std::async(
                    [i, backendType, rhiName]
                    {
                        String name = std::format("Sample {} ({})", i, rhiName);
                        WindowApplication<MutliEngineSample>::Builder()
                            .Title(name.c_str())
                            .RendererBackend(backendType)
                            .Build()
                            .Run();
                    }));
        }
    }

    for (auto& task : tasks)
    {
        task.wait();
    }
}
