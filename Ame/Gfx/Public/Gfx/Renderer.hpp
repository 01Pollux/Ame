#pragma once

#include <Core/Ame.hpp>
#include <Ecs/Universe.hpp>

#include <Core/Signals/Frame.hpp>

namespace Ame
{
    class EngineFrame;
    class FrameTimer;

    namespace Rhi
    {
        class Device;
        namespace Staging
        {
            class DeferredStagingManager;
        } // namespace Staging
    }     // namespace Rhi

    namespace Ecs::Component
    {
        struct Transform;
        struct Camera;
    } // namespace Ecs::Component

    namespace Gfx
    {
        namespace Cache
        {
            class CommonRenderPass;
        } // namespace Cache
        namespace RG
        {
            class Graph;
        } // namespace RG
    }     // namespace Gfx
} // namespace Ame

namespace Ame::Gfx
{
    class Renderer
    {
        using CameraRenderQuery = Ecs::UniqueQuery<
            const Ecs::Component::Transform,
            const Ecs::Component::Camera>;

    public:
        Renderer(
            EngineFrame&                          engineFrame,
            FrameTimer&                           frameTimer,
            Rhi::Device&                          rhiDevice,
            Rhi::Staging::DeferredStagingManager& stagingManager,
            Ecs::Universe&                        universe,
            RG::Graph&                            renderGraph,
            Cache::CommonRenderPass&              commonRenderPass);

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
        /// <summary>
        /// Flush all deferred uploads
        /// </summary>
        void FlushDeferredUploads();

        /// <summary>
        /// Run the render graph to render the frame
        /// </summary>
        void RunRenderGraph();

    private:
        Ref<EngineFrame>                          m_Frame;
        Ref<FrameTimer>                           m_Timer;
        Ref<Rhi::Device>                          m_Device;
        Ref<Ecs::Universe>                        m_Universe;
        Ref<Rhi::Staging::DeferredStagingManager> m_StagingManager;
        Ref<RG::Graph>                            m_Graph;
        Ref<Cache::CommonRenderPass>              m_CommonRenderPass;

        Signals::OnWorldChange::Handle m_OnWorldChange;

        Signals::OnUpdate::Handle     m_OnUpdate;
        Signals::OnStartFrame::Handle m_OnStartFrame;
        Signals::OnRender::Handle     m_OnRender;
        Signals::OnEndFrame::Handle   m_OnEndFrame;

        CameraRenderQuery m_CameraQuery;
    };
} // namespace Ame::Gfx