#include <Gfx/Renderer.hpp>

#include <Frame/Frame.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        Ptr<IFrame>  Frame,
        FrameTimer&  Timer,
        Rhi::Device& Device) :
        m_Frame(Frame),
        m_Timer(Timer),
        m_Device(Device),
        m_Graph(Timer, Device)
    {
        if (!Device.IsHeadless())
        {
            m_OnUpdate = {
                Frame->OnUpdate()
                    .ObjectSignal(),
                [this]
                { OnUpdate(); }
            };

            m_OnStartFrame = {
                Frame->OnStartFrame()
                    .ObjectSignal(),
                [this]
                { OnStartFrame(); }
            };

            m_OnRender = {
                Frame->OnRender()
                    .ObjectSignal(),
                [this]
                { OnRender(); }
            };

            m_OnEndFrame = {
                Frame->OnEndFrame()
                    .ObjectSignal(),
                [this]
                { OnEndFrame(); }
            };
        }
    }

    void Renderer::OnUpdate()
    {
        m_Graph.Update();
    }

    void Renderer::OnStartFrame()
    {
        if (!m_Device.get().ProcessEvents()) [[unlikely]]
        {
            m_Frame->Stop();
        }
        else
        {
            m_Device.get().BeginFrame();
        }
    }

    void Renderer::OnRender()
    {
        m_Graph.Execute();
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }
} // namespace Ame::Gfx