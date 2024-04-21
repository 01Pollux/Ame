#include <FlappyRocket/Engine.hpp>

#include <Ecs/Universe.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Geometry2D/Sprite.hpp>

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

        //

        auto Player = World.CreateEntity("Player");
        Player.AddComponent<Ecs::Component::Sprite>();
        Player.AddComponent<Ecs::Component::Transform>();
    }

    //

    void FlappyRocketGame::Update(
        const EngineTimer& Timer)
    {
        m_EcsUniverse->ProgressActiveWorld(Timer.GetDeltaTime());
    }
} // namespace Ame::FlappyRocket