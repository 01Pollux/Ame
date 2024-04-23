#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    class IFrame;
    class FrameTimer;

    namespace Ecs
    {
        struct Universe;
    } // namespace Ecs
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
            Ecs::Universe& EcsUniverse);

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
        Ref<IFrame>        m_Frame;
        Ref<FrameTimer>    m_Timer;
        Ref<Rhi::Device>   m_Device;
        Ref<Ecs::Universe> m_EcsUniverse;
    };
} // namespace Ame::Gfx