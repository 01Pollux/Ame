#pragma once

#include <Core/Ame.hpp>
#include <Gfx/Compositor/Signals.hpp>
#include <Gfx/Compositor/DrawCompositor.hpp>

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
    class EcsSystemHooks;
    struct EcsSystemDesc;

    class EntityCompositor
    {
    public:
        EntityCompositor(
            Rhi::Device&         rhiDevice,
            Ecs::Universe&       universe,
            const EcsSystemDesc& ecsDesc);

        EntityCompositor(const EntityCompositor&) = delete;
        EntityCompositor(EntityCompositor&&)      = default;

        EntityCompositor& operator=(const EntityCompositor&) = delete;
        EntityCompositor& operator=(EntityCompositor&&)      = default;

        ~EntityCompositor();

    public:
        /// <summary>
        /// Dispatches the camera to the render graph.
        /// </summary>
        void DispatchCamera(
            RG::Graph&                       renderGraph,
            const Ecs::Entity&               cameraEntity,
            const Ecs::Component::Camera&    cameraComponent,
            const Ecs::Component::Transform& cameraTransform);

    private:
        /// <summary>
        /// Returns the renderable entities.
        /// </summary>
        [[nodiscard]] std::vector<Ecs::Entity> GetRenderables(
            const Ecs::Entity& cameraEntity) const;

        /// <summary>
        /// Fetches entities and sorts them.
        /// </summary>
        void FetchAndSort(
            Signals::Data::DrawCompositorData& drawData);

        /// <summary>
        /// Executes the render graph and clears the compositor.
        /// </summary>
        void ExecuteAndClear(
            RG::Graph& renderGraph);

    public:
        AME_SIGNAL_INST(OnRenderCompose);

    private:
        UPtr<EcsSystemHooks> m_SystemHooks;
        DrawCompositor       m_DrawCompositor;
    };
} // namespace Ame::Gfx