#pragma once

#include <Engine/Engine.hpp>
#include <Engine/Timer.hpp>

namespace Ame::Ecs
{
    class Universe;
} // namespace Ame::Ecs

namespace Ame::FlappyRocket
{
    class FlappyRocketGame
    {
    public:
        FlappyRocketGame() = default;
        FlappyRocketGame(
            Ecs::Universe& EcsUniverse);

    public:
        /// <summary>
        /// Recreate the world
        /// </summary>
        void ResetWorld();

    public:
        /// <summary>
        /// Update the game
        /// </summary>
        void Update(
            const EngineTimer& Timer);

    private:
        Ecs::Universe* m_EcsUniverse = nullptr;
    };
} // namespace Ame::FlappyRocket