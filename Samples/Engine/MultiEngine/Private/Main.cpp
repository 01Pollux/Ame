#include <future>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

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

        co_return;
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

    for (size_t i = 0; i < NumberOfApps; i++)
    {
        tasks.emplace_back(
            std::async(
                [i]
                {
                    String name = std::format("Sample {}", i);
                    WindowApplication<MutliEngineSample>::Builder()
                        .Title(name.c_str())
                        .RendererBackend(Rhi::DeviceType::DirectX12)
                        .Build()
                        .Run();
                }));
    }

    for (auto& task : tasks)
    {
        task.wait();
    }
}
