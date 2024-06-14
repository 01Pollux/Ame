#pragma once

#include <Core/Ame.hpp>

#include <Gfx/Compositor/Signals.hpp>
#include <Gfx/Compositor/EntityDrawInfo.hpp>
#include <Gfx/Compositor/DrawCompositor.hpp>
#include <Gfx/Compositor/EcsWorldResourcesDesc.hpp>

#include <WorldNTree/WorldNTree.hpp>

namespace Ame::RG
{
    class Graph;
} // namespace Ame::RG

namespace Ame
{
    namespace Ecs
    {
        class Entity;
        class Universe;
        namespace Component
        {
            struct Camera;
            struct Transform;
        } // namespace Component
    }     // namespace Ecs
    namespace RG
    {
        class Graph;
    } // namespace RG
} // namespace Ame

namespace Ame::Gfx
{
    class EcsWorldResources;

    class EntityCompositor
    {
    public:
        EntityCompositor(
            Rhi::Device&                 rhiDevice,
            Ecs::Universe&               universe,
            RG::Graph&                   renderGraph,
            Extensions::WorldOctTreeBox& worldOctTreeBox,
            const EcsWorldResourcesDesc& ecsDesc = {});

        EntityCompositor(const EntityCompositor&) = delete;
        EntityCompositor(EntityCompositor&&)      = default;

        EntityCompositor& operator=(const EntityCompositor&) = delete;
        EntityCompositor& operator=(EntityCompositor&&)      = default;

        ~EntityCompositor();

    public:
        /// <summary>
        /// Updates the render graph and upload transform and per entities data.
        /// </summary>
        void UpdateGraph();

        /// <summary>
        /// Dispatches the camera to the render graph.
        /// </summary>
        void RenderGraph(
            const Ecs::Entity&               cameraEntity,
            const Ecs::Component::Camera&    cameraComponent,
            const Ecs::Component::Transform& cameraTransform);

    private:
        /// <summary>
        /// Flushes and uploads resources to the render graph.
        /// </summary>
        void FlushAndUploadResourcesToGraph();

    private:
        /// <summary>
        /// Returns the renderable entities.
        /// </summary>
        [[nodiscard]] std::vector<EntityDrawInfo> GetRenderables(
            const Ecs::Component::Camera& cameraComponent) const;

        /// <summary>
        /// Fetches entities and sorts them.
        /// </summary>
        void FetchAndSortDrawData(
            Signals::Data::DrawCompositorData& drawData);

        /// <summary>
        /// Executes the render graph and clears the compositor.
        /// </summary>
        void ExecuteAndClearGraph();

    public:
        AME_SIGNAL_INST(OnRenderCompose);

    private:
        Ref<RG::Graph>                   m_Graph;
        Ref<Extensions::WorldOctTreeBox> m_WorldOctTreeBox;

        UPtr<EcsWorldResources> m_EcsResources;
        DrawCompositor          m_DrawCompositor;
    };
} // namespace Ame::Gfx