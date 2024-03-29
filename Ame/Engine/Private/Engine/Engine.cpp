#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    BaseEngine::BaseEngine(
        Rhi::Device RhiDevice) :
        m_RhiDevice(std::move(RhiDevice))
    {
    }

    BaseEngine::~BaseEngine() = default;

    void BaseEngine::Run()
    {
        Log::Engine().Trace("Initializing Engine...");

        m_Logic.Initialize();
        Initialize();

        Log::Engine().Trace("Engine Initialized");

        m_Timer.Reset();
        if (m_RhiDevice.GetGraphicsAPI() != Rhi::GraphicsAPI::Null)
        {
            RenderLoop();
        }
        else
        {
            HeadlessLoop();
        }

        Log::Engine().Trace("Shutting down Engine...");

        Shutdown();
        m_Logic.Shutdown();

        Log::Engine().Trace("Engine Shutdown");
    }

    void BaseEngine::Close()
    {
        m_Logic.Stop();
    }

    //

    void BaseEngine::RenderLoop()
    {
        while (m_Logic.IsRunning() && m_RhiDevice.ProcessEvents()) [[likely]]
        {
            m_RhiDevice.BeginFrame();
            m_Logic.Tick(m_Timer, &m_RhiDevice);
            m_RhiDevice.EndFrame();
        }
    }

    void BaseEngine::HeadlessLoop()
    {
        while (m_Logic.IsRunning())
        {
            m_Logic.Tick(m_Timer);
        }
    }
} // namespace Ame
