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
        static constexpr const StringU8View WorldName  = "Flappy Rocket";
        static constexpr const StringU8View PlayerName = "Player";

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