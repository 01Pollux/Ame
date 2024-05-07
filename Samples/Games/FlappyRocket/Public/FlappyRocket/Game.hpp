#pragma once

#include <Gfx/Renderer.hpp>
#include <Ecs/Universe.hpp>

namespace Ame::FlappyRocket
{
    class FlappyRocketGame
    {
        static inline const String WorldName  = "Flappy Rocket";
        static inline const String PlayerName = "Player";
        static inline const String CameraName = "Camera";

    public:
        FlappyRocketGame() = default;
        FlappyRocketGame(
            Gfx::Renderer& Renderer,
            Ecs::Universe& EcsUniverse);

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
            Gfx::RG::Graph& RenderGraph);

    private:
        Ecs::Universe* m_EcsUniverse = nullptr;
    };
} // namespace Ame::FlappyRocket