#pragma once

#include <Ecs/Universe.hpp>

namespace Ame::Gfx
{
    namespace Cache
    {
        class PipelineLayoutCache;
    } // namespace Cache
    namespace RG
    {
        class Graph;
    } // namespace RG
    class Renderer;
} // namespace Ame::Gfx

namespace Ame::FlappyRocket
{
    class FlappyRocketGame
    {
        static inline const String WorldName  = "Flappy Rocket";
        static inline const String PlayerName = "Player";
        static inline const String CameraName = "Camera";

    public:
        FlappyRocketGame(
            Ecs::Universe&                   EcsUniverse,
            Gfx::Renderer&                   Renderer,
            Gfx::Cache::PipelineLayoutCache& LayoutCache);

    public:
        /// <summary>
        /// Remove the old world and recreate a new one
        /// </summary>
        void ResetWorld();

        /// <summary>
        /// Add all entities to the world for the game
        /// </summary>
        void AddAllEntities();

    private:
        /// <summary>
        /// Setup the render graph for the game
        /// </summary>
        void SetupRenderGraph(
            Gfx::RG::Graph&                  RenderGraph,
            Gfx::Cache::PipelineLayoutCache& LayoutCache);

    private:
        Ecs::Universe* m_EcsUniverse = nullptr;
    };
} // namespace Ame::FlappyRocket