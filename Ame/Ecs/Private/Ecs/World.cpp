#include <Ecs/World.hpp>
#include <EcsUtil/Entity.hpp>

#include <Log/Wrapper.hpp>

#include <mutex>

namespace Ame::Ecs
{
    /// <summary>
    /// The mutex for initializing flecs world.
    /// It is used to prevent multiple threads from initializing the world at the same time.
    /// </summary>
    static std::mutex g_FlecsMutex;

    struct WorldName
    {
        StringU8 Name;
    };

    World::World(
        const StringU8& Name)
    {
        {
            std::lock_guard Lock(g_FlecsMutex);
            m_World = std::make_unique<flecs::world>();
            m_World->set(WorldName{ Name });
        }
        RegisterModules();
    }

    World::World(World&& Other) noexcept :
        m_World(std::move(Other.m_World))
    {
    }

    World& World::operator=(
        World&& Other) noexcept
    {
        if (this != &Other)
        {
            if (m_World)
            {
                std::lock_guard Lock(g_FlecsMutex);
                m_World.reset();
            }
            m_World = std::move(Other.m_World);
        }
        return *this;
    }

    World::~World()
    {
        if (m_World)
        {
            std::lock_guard Lock(g_FlecsMutex);
            m_World.reset();
        }
    }

    //

    Entity World::CreateEntity(
        const StringU8& Name,
        const Entity&   Parent)
    {
        auto FlecsEntity = m_World->entity();
        if (Parent)
        {
            FlecsEntity.child_of(Parent);
        }
        FlecsEntity.set_name(Name.c_str());
        return Entity(FlecsEntity);
    }

    void World::DestroyEntity(
        const Entity& EcsEntity,
        bool          WithChildren)
    {
        auto& FlecsEntity = EcsEntity.GetFlecsEntity();

        Log::Ecs().Assert(FlecsEntity.is_valid(), "Entity is not valid.");
        Log::Ecs().Assert(FlecsEntity.world() == *m_World, "Entity is not from this world.");

        if (!WithChildren)
        {
            // Move children to parent of parent.
            // and rename them to avoid name conflicts.
            auto OurParent = FlecsEntity.parent();
            for (auto& Child : EcsEntity.GetChildren())
            {
                Child.SetParent(OurParent);
            }
        }

        FlecsEntity.destruct();
    }

    std::vector<Entity> World::GetEntities(
        const StringU8& Name)
    {
        return {};
    }

    //

    StringU8 World::GetUniqueEntityName(
        const char*   Name,
        const Entity& Parent) const
    {
        return EcsUtil::GetUniqueEntityName(*m_World, Name, Parent.GetFlecsEntity());
    }

    //

    void World::Progress(
        double DeltaTime)
    {
        m_World->progress(DeltaTime);
    }
} // namespace Ame::Ecs