#pragma once

#include <Core/Ame.hpp>
#include <Gfx/RG/Graph.hpp>
#include <Ecs/Universe.hpp>

#include <Core/Signals/Frame.hpp>

namespace Ame
{
    class EngineFrame;
    class FrameTimer;

    namespace Rhi
    {
        class Device;
    } // namespace Rhi

    namespace Ecs::Component
    {
        struct Transform;
        struct Camera;
    } // namespace Ecs::Component
} // namespace Ame

namespace Ame::Gfx
{
    class Renderer
    {
        using CameraRenderQuery = Ecs::UniqueQuery<const Ecs::Component::Transform, const Ecs::Component::Camera>;

    public:
        Renderer(
            EngineFrame&   Frame,
            FrameTimer&    Timer,
            Rhi::Device&   Device,
            Ecs::Universe& Universe);

    public:
        /// <summary>
        /// Get the render graph
        /// </summary>
        [[nodiscard]] const RG::Graph& GetRenderGraph() const;

        /// <summary>
        /// Get the render graph
        /// </summary>
        [[nodiscard]] RG::Graph& GetRenderGraph();

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
        Ref<EngineFrame>   m_Frame;
        Ref<FrameTimer>    m_Timer;
        Ref<Rhi::Device>   m_Device;
        Ref<Ecs::Universe> m_Universe;

        Signals::OnWorldChange::Handle m_OnWorldChange;

        Signals::OnUpdate::Handle     m_OnUpdate;
        Signals::OnStartFrame::Handle m_OnStartFrame;
        Signals::OnRender::Handle     m_OnRender;
        Signals::OnEndFrame::Handle   m_OnEndFrame;

        RG::Graph         m_Graph;
        CameraRenderQuery m_CameraQuery;
    };
} // namespace Ame::Gfx