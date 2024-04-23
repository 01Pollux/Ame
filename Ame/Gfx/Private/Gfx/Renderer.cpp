#include <Gfx/Renderer.hpp>

#include <Frame/Frame.hpp>
#include <Ecs/Universe.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        IFrame&        Frame,
        FrameTimer&    Timer,
        Rhi::Device&   Device,
        Ecs::Universe& EcsUniverse) :
        m_Frame(Frame),
        m_Timer(Timer),
        m_Device(Device),
        m_EcsUniverse(EcsUniverse)
    {
        if (!Device.IsHeadless())
        {
            Frame.OnUpdate()
                .ObjectSignal()
                .Listen([this]
                        { OnUpdate(); });

            Frame.OnStartFrame()
                .ObjectSignal()
                .Listen([this]
                        { OnStartFrame(); });

            Frame.OnRender()
                .ObjectSignal()
                .Listen([this]
                        { OnRender(); });

            Frame.OnEndFrame()
                .ObjectSignal()
                .Listen([this]
                        { OnEndFrame(); });
        }
    }

    void Renderer::OnUpdate()
    {
        m_EcsUniverse.get().ProgressActiveWorld(m_Timer.get().GetDeltaTime());
    }

    void Renderer::OnStartFrame()
    {
        if (!m_Device.get().ProcessEvents())
        {
            m_Frame.get().Stop();
        }
        else
        {
            m_Device.get().BeginFrame();
        }
    }

    void Renderer::OnRender()
    {
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }
} // namespace Ame::Gfx