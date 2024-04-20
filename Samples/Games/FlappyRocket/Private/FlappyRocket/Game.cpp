#include <FlappyRocket/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Ecs::Universe& EcsUniverse) :
        m_EcsUniverse(EcsUniverse)
    {
    }
} // namespace Ame::FlappyRocket