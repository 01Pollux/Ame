#include <Engine/Logic.hpp>
#include <Engine/Engine.hpp>

namespace Ame
{
    void EngineLogic::StartFrame(
        BaseEngine& Engine)
    {
        m_OnStartFrame.Broadcast(Engine);
    }

    Co::result<void> EngineLogic::TickRender(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        BaseEngine&               Engine)
    {
        m_OnRender.Broadcast(Engine);
        m_OnPostRender.Broadcast(Engine);
        co_return;
    }

    void EngineLogic::Tick(
        BaseEngine& Engine)
    {
        m_OnUpdate.Broadcast(Engine);
        m_OnPostUpdate.Broadcast(Engine);
    }

    void EngineLogic::EndFrame(
        BaseEngine& Engine)
    {
        m_OnEndFrame.Broadcast(Engine);
    }

    void EngineLogic::Stop()
    {
        m_IsRunning = false;
    }

    //

    void EngineLogic::Initialize()
    {
    }

    void EngineLogic::Shutdown()
    {
    }
} // namespace Ame
