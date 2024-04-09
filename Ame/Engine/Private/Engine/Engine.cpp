#include <Engine/Engine.hpp>

#include <Core/Subsystem/Coroutine.hpp>
#include <Engine/Subsystem/Timer.hpp>
#include <Rhi/Subsystem/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void BaseEngine::Run()
    {
        DoInitialize();

        if (HasSubsystem<Rhi::DeviceSubsystem>())
        {
            auto& RhiDevice = GetSubsystem<Rhi::DeviceSubsystem>();
            if (!RhiDevice.IsHeadless())
            {
                DoRenderLoop(RhiDevice);
                DoShutdown();
                return;
            }
        }

        DoHeadlessLoop();
        DoShutdown();
    }

    void BaseEngine::Close()
    {
        m_IsRunning = false;
    }

    //

    void BaseEngine::DoInitialize()
    {
        Log::Engine().Trace("Initializing Engine...");
        Initialize();
        Log::Engine().Trace("Engine Initialized");
    }

    void BaseEngine::Initialize()
    {
    }

    //

    void BaseEngine::Shutdown()
    {
    }

    void BaseEngine::DoShutdown()
    {
        Log::Engine().Trace("Shutting down Engine...");
        Shutdown();
        Log::Engine().Trace("Engine Shutdown");
    }

    //

    void BaseEngine::DoRenderLoop(
        Rhi::Device& RhiDevice)
    {
        auto& Runtime = *GetSubsystem<CoroutineSubsystem>();
        auto& Timer   = GetSubsystem<TimerSubsystem>();
        Timer.Reset();

        auto DoRender = [this](Co::executor_tag, Co::executor& Executor) -> Co::result<void>
        {
            m_OnRender.Broadcast(*this);
            m_OnPostRender.Broadcast(*this);
            co_return;
        };

        while (m_IsRunning && RhiDevice.ProcessEvents()) [[likely]]
        {
            Timer.Tick();

            RhiDevice.BeginFrame();
            m_OnStartFrame.Broadcast(*this);

            auto Tick = DoRender({}, *Runtime.thread_pool_executor());
            m_OnUpdate.Broadcast(*this);
            m_OnPostUpdate.Broadcast(*this);

            Tick.get();

            m_OnEndFrame.Broadcast(*this);
            RhiDevice.EndFrame();
        }
    }

    void BaseEngine::DoHeadlessLoop()
    {
        auto& Timer = GetSubsystem<TimerSubsystem>();
        while (m_IsRunning)
        {
            Timer.Tick();
            m_OnStartFrame.Broadcast(*this);

            m_OnUpdate.Broadcast(*this);
            m_OnPostUpdate.Broadcast(*this);

            m_OnEndFrame.Broadcast(*this);
        }
    }
} // namespace Ame
