#include <Engine/Logic.hpp>
#include <Engine/Engine.hpp>

namespace Ame
{
    void EngineLogic::StartFrame(
        BaseEngine& Engine)
    {
        m_OnStartFrame.Broadcast(Engine);
    }

    void EngineLogic::Tick(
        Co::runtime& Runtime,
        BaseEngine&  Engine,
        Rhi::Device* RhiDevice)
    {
        auto Executor = Runtime.thread_pool_executor();

        auto Task = Executor->submit(
            [&]() -> Co::result<void>
            {
                Tick(Engine);
                co_return;
            });

        m_OnRender.Broadcast(Engine);
        m_OnPostRender.Broadcast(Engine);

        Task.wait();
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
