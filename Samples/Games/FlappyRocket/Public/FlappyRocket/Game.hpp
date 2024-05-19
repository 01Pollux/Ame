#pragma once

#include <Ecs/Universe.hpp>

namespace Ame
{
    namespace Gfx
    {
        namespace Cache
        {
            class PipelineStateCache;
            class ShaderCache;
        } // namespace Cache
        namespace RG
        {
            class Graph;
        } // namespace RG
        class Renderer;
    } // namespace Gfx
    namespace Asset
    {
        class Storage;
    } // namespace Asset
} // namespace Ame

namespace Ame::FlappyRocket
{
    class FlappyRocketGame
    {
        static inline const String WorldName  = "Flappy Rocket";
        static inline const String PlayerName = "Player";
        static inline const String CameraName = "Camera";

    public:
        FlappyRocketGame(
            Rhi::Device&                    Device,
            Ecs::Universe&                  EcsUniverse,
            Gfx::Renderer&                  Renderer,
            Gfx::Cache::PipelineStateCache& PipelineStateCache,
            Gfx::Cache::ShaderCache&        ShaderCache);

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
            Gfx::RG::Graph&                 RenderGraph,
            Gfx::Cache::PipelineStateCache& PipelineStateCache);

    private:
        Rhi::Device*             m_Device      = nullptr;
        Ecs::Universe*           m_EcsUniverse = nullptr;
        Gfx::Cache::ShaderCache* m_ShaderCache = nullptr;
    };
} // namespace Ame::FlappyRocket