#include <Framework/EntryPoint.hpp>
#include <Framework/Headless.hpp>

#include <Subsystem/Core/Frame.hpp>
#include <Subsystem/Core/Timer.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class SimpleLogEngine : public IoCContainer
{
public:
    Co::result<void> Run()
    {
        Initialize();

        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        while (engineFrame.IsRunning())
        {
            engineFrame.Tick();
        }

        co_return;
    }

private:
    Co::null_result Initialize()
    {
        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        auto& frameTimer  = GetSubsystem<FrameTimerSubsystem>();
        auto& runtime     = GetSubsystem<CoroutineSubsystem>();
        auto& timerQueue  = *runtime.timer_queue();

        co_await timerQueue.make_delay_object(std::chrono::seconds(1), runtime.thread_pool_executor());

        co_await Co::resume_on(*engineFrame.GetStartFrameExecutor());
        Log::Client().Info("Start Frame time:{}", frameTimer.GetEngineTime());

        co_await Co::resume_on(*engineFrame.GetTickFrameExecutor());
        Log::Client().Info("Tick Frame time:{}", frameTimer.GetEngineTime());

        co_await Co::resume_on(*engineFrame.GetStartFrameExecutor());
        Log::Client().Info("Start Frame time:{}", frameTimer.GetEngineTime());

        co_await Co::resume_on(*engineFrame.GetEndFrameExecutor());
        Log::Client().Info("End Frame time:{}", frameTimer.GetEngineTime());

        engineFrame.Stop();
    }
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Client.log");
    Log::Logger::Register(Log::Names::c_Client, "Ignore.log");

    Log::Client().Info("Hello, World!");
    Log::Editor().Warning("Ignore");

    HeadlessApplication<SimpleLogEngine>::Builder()
        .Build()
        .Run();

    Log::Client().Error("Goodbye, World!");
}
