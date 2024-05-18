
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Frame/Subsystem/Timer.hpp>
#include <Window/Window.hpp>

#include <Log/Wrapper.hpp>

static constexpr uint32_t NumberOfApps = 5;

using namespace Ame;

class MutliEngineSample : public BaseEngine
{
public:
    using BaseEngine::BaseEngine;

protected:
    void Initialize() override
    {
        BaseEngine::Initialize();

        Log::Engine().Trace("Initializing Sample...");
        InitializeWindow(
            GetSubsystem<CoroutineSubsystem>(),
            GetSubsystem<Rhi::DeviceSubsystem>());
    }

private:
    void InitializeWindow(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        m_Title = RhiDevice.GetWindow().GetTitle();

        auto TimerQueue = Coroutine.timer_queue();
        m_TitleTimer    = TimerQueue->make_timer(
            std::chrono::seconds(1),
            std::chrono::seconds(1),
            Coroutine.thread_pool_executor(),
            [this]()
            {
                UpdateTitle(
                    GetSubsystem<FrameTimerSubsystem>(),
                    GetSubsystem<Rhi::DeviceSubsystem>());
            });
    }

    void UpdateTitle(
        FrameTimer&  Timer,
        Rhi::Device& RhiDevice)
    {
        double FPS = 1.0 / Timer.GetDeltaTime();
        RhiDevice.GetWindow().SetTitle(std::format("{} - FPS: {:.2f}", m_Title, FPS));
    }

private:
    String    m_Title;
    Co::timer m_TitleTimer;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    auto Runtime(std::make_shared<Co::runtime>());
    auto Executor = Runtime->thread_pool_executor();

    std::vector<Co::result<void>> Tasks;
    Tasks.reserve(NumberOfApps);

    for (size_t i = 0; i < NumberOfApps; i++)
    {
        String Name = String::formatted("Sample {}", i);
        Tasks.emplace_back(Executor->submit(
            [Runtime, Name]
            {
                WindowApplication<MutliEngineSample>::Builder()
                    .Title(Name.c_str())
                    .Runtime(std::move(Runtime))
                    .RendererBackend(Rhi::DeviceType::DirectX12)
                    .Build()
                    .Run();
            }));
    }

    Co::when_all(Executor, Tasks.begin(), Tasks.end()).run().get();
}
