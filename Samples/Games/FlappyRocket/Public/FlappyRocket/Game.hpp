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

    private:
        std::reference_wrapper<Ecs::Universe> m_EcsUniverse;
    };
} // namespace Ame::FlappyRocket