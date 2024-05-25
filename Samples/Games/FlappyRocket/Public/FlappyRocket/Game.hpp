#pragma once

#include <Ecs/Universe.hpp>

namespace Ame
{
    namespace Gfx
    {
        namespace Cache
        {
            class ShaderCache;
            class CommonShader;
            class CommonPipelineState;
            class MaterialBindingCache;
        } // namespace Cache
        namespace RG
        {
            class Graph;
        } // namespace RG
        class Gfx;
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
        static inline const String c_WorldName  = "Flappy Rocket";
        static inline const String c_PlayerName = "Player";
        static inline const String c_CameraName = "Camera";

    public:
        FlappyRocketGame(
            Rhi::Device&                      device,
            Ecs::Universe&                    ecsUniverse,
            Gfx::Renderer&                    renderer,
            Gfx::Cache::ShaderCache&          shaderCache,
            Gfx::Cache::CommonShader&         commonShaders,
            Gfx::Cache::CommonPipelineState&  commonPipelines,
            Gfx::Cache::MaterialBindingCache& materialCache);

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
            Gfx::RG::Graph&                   renderGraph,
            Gfx::Cache::CommonShader&         commonShaders,
            Gfx::Cache::CommonPipelineState&  commonPipelines,
            Gfx::Cache::MaterialBindingCache& materialCache);

    private:
        Rhi::Device*             m_Device      = nullptr;
        Ecs::Universe*           m_EcsUniverse = nullptr;
        Gfx::Cache::ShaderCache* m_ShaderCache = nullptr;
    };
} // namespace Ame::FlappyRocket