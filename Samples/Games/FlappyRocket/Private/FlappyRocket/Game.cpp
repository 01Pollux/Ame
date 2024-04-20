#include <FlappyRocket/Engine.hpp>

#include <Ecs/Universe.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    FlappyRocketGame::FlappyRocketGame(
        Ecs::Universe& EcsUniverse) :
        m_EcsUniverse(&EcsUniverse)
    {
    }

    //

    void FlappyRocketGame::ResetWorld()
    {
        m_EcsUniverse->RemoveWorld("Flappy Rocket");
        auto& World = m_EcsUniverse->CreateWorld("Flappy Rocket");
        m_EcsUniverse->SetActiveWorld(World);

        World.CreateEntity("Player");
    }

    //

    void FlappyRocketGame::Update(
        const EngineTimer& Timer)
    {
        m_EcsUniverse->ProgressActiveWorld(Timer.GetDeltaTime());
    }
} // namespace Ame::FlappyRocket