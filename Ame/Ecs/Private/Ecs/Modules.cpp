#include <Ecs/World.hpp>

#include <Ecs/Module/Standard.hpp>

namespace Ame::Ecs
{
    void World::RegisterModules()
    {
#ifndef AME_DIST
        m_World->import <flecs::monitor>();
        m_World->emplace<flecs::Rest>();
#endif

        m_World->import <Module::StandardModule>();
    }
} // namespace Ame::Ecs