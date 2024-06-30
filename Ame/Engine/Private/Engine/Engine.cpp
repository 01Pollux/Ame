#include <Engine/Engine.hpp>

#include <Subsystem/Core/FrameTimer.hpp>
#include <Subsystem/Core/EngineFrame.hpp>

namespace Ame
{
    [[nodiscard]] std::vector<Ref<IEngineTick>> GetAllTickables(
        kgr::container& container)
    {
        auto allTickablesRange = container.service<OverrideRangeSubsystem<EngineTickSubsystem>>();
        return { allTickablesRange.begin(), allTickablesRange.end() };
    }

    Co::result<void> BaseAmeEngine::Run()
    {
        FrameTimer&  frameTimer  = m_Container.service<FrameTimerSubsystem>();
        EngineFrame& engineFrame = m_Container.service<EngineFrameSubsystem>();
        Co::runtime& runtime     = m_Container.service<CoroutineSubsystem>();

        co_await Initialize();

        //

        std::vector allTickables = GetAllTickables(m_Container);

        std::vector<Co::result<void>> allTasks;
        allTasks.reserve(allTickables.size());

        //

        while (engineFrame.IsRunning())
        {
            frameTimer.Tick();

            for (auto& tickable : allTickables)
            {
                allTasks.emplace_back(tickable.get().Tick(runtime));
            }

            co_await Co::when_all(runtime.inline_executor(), allTasks.begin(), allTasks.end()).run();
            allTasks.clear();
        }

        //

        co_await Shutdown();
    }

    Co::result<void> BaseAmeEngine::Initialize()
    {
        co_return;
    }

    Co::result<void> BaseAmeEngine::Shutdown()
    {
        co_return;
    }
} // namespace Ame