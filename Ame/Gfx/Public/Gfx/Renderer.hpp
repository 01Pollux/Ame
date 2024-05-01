#pragma once

#include <Core/Ame.hpp>
#include <Gfx/RG/Graph.hpp>

#include <Core/Signals/Frame.hpp>

namespace Ame
{
    class IFrame;
    class FrameTimer;

    namespace Rhi
    {
        class Device;
    } // namespace Rhi
} // namespace Ame

namespace Ame::Gfx
{
    class Renderer
    {
    public:
        Renderer(
            IFrame&        Frame,
            FrameTimer&    Timer,
            Rhi::Device&   Device,
            Ecs::Universe& Universe);

    private:
        /// <summary>
        /// Update the renderer and all its components such as the camera, the scene, lights, etc.
        /// </summary>
        void OnUpdate();

        /// <summary>
        /// Called before the frame begins
        /// </summary>
        void OnStartFrame();

        /// <summary>
        /// Called when the frame is being rendered
        /// </summary>
        void OnRender();

        /// <summary>
        /// Called after the frame ends
        /// </summary>
        void OnEndFrame();

    private:
        Ref<IFrame>      m_Frame;
        Ref<FrameTimer>  m_Timer;
        Ref<Rhi::Device> m_Device;

        Signals::OnUpdate::Handle     m_OnUpdate;
        Signals::OnStartFrame::Handle m_OnStartFrame;
        Signals::OnRender::Handle     m_OnRender;
        Signals::OnEndFrame::Handle   m_OnEndFrame;

        RG::Graph m_Graph;
    };
} // namespace Ame::Gfx