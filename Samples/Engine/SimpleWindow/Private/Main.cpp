
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/DeviceWindowManager.hpp>

#include <Log/Wrapper.hpp>

enum class RenderLoopStrategy
{
    Inlined,      // Render and tick in the same thread
    Threaded,     // Render and tick in different threads
    ThreadPooled, // Render and tick in different threads with thread pool
};

using namespace Ame;
class SimpleWindowEngine : public IoCContainer
{
public:
    SimpleWindowEngine(
        RenderLoopStrategy renderLoopStrategy) :
        m_RenderLoopStrategy(renderLoopStrategy)
    {
    }

    Co::result<void> Run()
    {
        Initialize();

        auto& engineFrame  = GetSubsystem<EngineFrameSubsystem>();
        auto& rhiDevice    = GetSubsystem<Rhi::DeviceSubsystem>();
        auto  thisExecutor = GetSubsystem<CoroutineSubsystem>().inline_executor();

        Log::Client().Info("Main thread: {}", std::this_thread::get_id());

        while (engineFrame.IsRunning())
        {
            auto renderExecutor = GetRenderExecutor();

            auto frameTick   = thisExecutor->submit(std::bind(&EngineFrame::Tick, &engineFrame));
            auto frameRender = renderExecutor->submit(std::bind(&Rhi::Device::Tick, &rhiDevice));

            Co::when_all(thisExecutor, std::move(frameTick), std::move(frameRender)).run().wait();
        }

        co_return;
    }

private:
    Co::null_result Initialize()
    {
        UpdateOnFrames();
        co_return;
    }

private:
    [[nodiscard]] Ptr<Co::executor> GetUpdateExecutor()
    {
        return GetSubsystem<CoroutineSubsystem>().inline_executor();
    }

    [[nodiscard]] Ptr<Co::executor> GetRenderExecutor()
    {
        switch (m_RenderLoopStrategy)
        {
        case RenderLoopStrategy::Inlined:
            return GetSubsystem<CoroutineSubsystem>().inline_executor();
            break;
        case RenderLoopStrategy::Threaded:
            return GetSubsystem<CoroutineSubsystem>().thread_executor();
            break;
        case RenderLoopStrategy::ThreadPooled:
            return GetSubsystem<CoroutineSubsystem>().thread_pool_executor();
            break;
        default:
            std::unreachable();
        }
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
    RenderLoopStrategy m_RenderLoopStrategy;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Engine.log");
    Log::Logger::Register(Log::Names::c_Window, "Window.log");

    WindowApplication<SimpleWindowEngine>::Builder()
        .RendererBackend(Rhi::DeviceType::DirectX12)
        .ValidationLayer()
        .Title("Simple Window")
        .Build(RenderLoopStrategy::ThreadPooled)
        .Run();
}
