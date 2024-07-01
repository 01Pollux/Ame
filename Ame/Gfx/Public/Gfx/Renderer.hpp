#pragma once

#include <Core/Ame.hpp>
#include <Frame/EngineTick.hpp>

#include <Ecs/Universe.hpp>
#include <Gfx/Compositor.hpp>

#include <Math/Camera.hpp>
#include <Math/Transform.hpp>

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

    namespace RG
    {
        class Graph;
    } // namespace RG

    namespace Gfx
    {
        namespace Cache
        {
            class CommonRenderPass;
        } // namespace Cache
    }     // namespace Gfx
} // namespace Ame

namespace Ame::Gfx
{
    class Renderer : public IEngineTick
    {
        using CameraRenderQuery = Ecs::UniqueQuery<
            const Ecs::Component::Transform,
            const Ecs::Component::Camera>;

    public:
        Renderer(
            EngineFrame&             engineFrame,
            FrameTimer&              frameTimer,
            Rhi::Device&             rhiDevice,
            Ecs::Universe&           universe,
            RG::Graph&               renderGraph,
            EntityCompositor&        entityCompositor,
            Cache::CommonRenderPass& commonRenderPass);

    public:
        [[nodiscard]] Co::result<void> Tick(
            Co::runtime& runtime) override;

    private:
        /// <summary>
        /// Called when the world changes
        /// </summary>
        void OnWorldChange(
            const Signals::Data::WorldChangeData& changeData);

    private:
        /// <summary>
        /// Create an empty render graph
        /// </summary>
        void CreateEmptyGraph();

        /// <summary>
        /// Build the render graph if needed
        /// </summary>
        void BuildRenderGraph();

        /// <summary>
        /// Run the render graph to render the frame
        /// </summary>
        void RunRenderGraph();

    private:
        Ref<EngineFrame>             m_Frame;
        Ref<FrameTimer>              m_Timer;
        Ref<Rhi::Device>             m_Device;
        Ref<Ecs::Universe>           m_Universe;
        Ref<RG::Graph>               m_Graph;
        Ref<EntityCompositor>        m_EntityCompositor;
        Ref<Cache::CommonRenderPass> m_CommonRenderPass;

        Signals::ScopedConnection m_OnWorldChange;

        CameraRenderQuery m_CameraQuery;
        Math::Camera      m_DefaultCamera;
    };
} // namespace Ame::Gfx