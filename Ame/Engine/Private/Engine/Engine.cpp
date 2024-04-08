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
        m_Logic.Stop();
    }

    //

    void BaseEngine::DoInitialize()
    {
        Log::Engine().Trace("Initializing Engine...");

        m_Logic.Initialize();
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
        m_Logic.Shutdown();

        Log::Engine().Trace("Engine Shutdown");
    }

    //

    void BaseEngine::DoRenderLoop(
        Rhi::Device& RhiDevice)
    {
        auto& Runtime = *GetSubsystem<CoroutineSubsystem>();
        auto& Timer   = GetSubsystem<TimerSubsystem>();
        Timer.Reset();

        while (m_Logic.IsRunning() && RhiDevice.ProcessEvents()) [[likely]]
        {
            Timer.Tick();

            RhiDevice.BeginFrame();
            m_Logic.StartFrame(*this);

            auto Tick = m_Logic.TickRender({}, *Runtime.thread_pool_executor(), *this);
            m_Logic.Tick(*this);
            Tick.get();

            m_Logic.EndFrame(*this);
            RhiDevice.EndFrame();
        }
    }

    void BaseEngine::DoHeadlessLoop()
    {
        auto& Timer = GetSubsystem<TimerSubsystem>();
        while (m_Logic.IsRunning())
        {
            Timer.Tick();

            m_Logic.StartFrame(*this);
            m_Logic.Tick(*this);
            m_Logic.EndFrame(*this);
        }
    }
} // namespace Ame
