#pragma once

#include <Engine/Engine.hpp>

namespace Ame::Ecs
{
    class Universe;
} // namespace Ame::Ecs

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
        Ecs::Universe* m_EcsUniverse = nullptr;
    };
} // namespace Ame::FlappyRocket