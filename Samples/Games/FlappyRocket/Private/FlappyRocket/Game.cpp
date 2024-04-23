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
        m_EcsUniverse->RemoveWorld(WorldName);
        auto& World = m_EcsUniverse->CreateWorld(WorldName);
        m_EcsUniverse->SetActiveWorld(World);
    }

    void FlappyRocketGame::AddAllEntities()
    {
        auto& World = *m_EcsUniverse->GetActiveWorld();

        auto Player = World.CreateEntity(PlayerName);
        Player.AddComponent<Ecs::Component::Sprite>();
        Player.AddComponent<Ecs::Component::Transform>();
    }
} // namespace Ame::FlappyRocket