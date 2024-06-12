#include <Ecs/Universe.hpp>
#include <Frame/EngineFrame.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
    Universe::Universe(
        EngineFrame& engineFrame,
        FrameTimer&  frameTimer) :
        m_OnUpdate(
            engineFrame.OnUpdate(
                [this, &frameTimer]
                { ProgressActiveWorld(frameTimer.GetDeltaTime()); }))
    {
    }

    Universe::Universe(
        Universe&& other) noexcept :
        m_OnUpdate(std::move(other.m_OnUpdate)),
        m_Worlds(std::move(other.m_Worlds)),
        m_ActiveWorld(std::exchange(other.m_ActiveWorld, nullptr))
    {
    }

    Universe::~Universe()
    {
        InvokeChangeWorld(nullptr);
    }

    //

    World& Universe::CreateWorld(
        const String& name)
    {
#ifdef AME_DEBUG
        if (m_Worlds.contains(name))
        {
            Log::Ecs().Error("Universe::CreateWorld: World already exists");
            return m_Worlds.at(name);
        }
#endif
        return m_Worlds.emplace(name, World{ name }).first->second;
    }

    void Universe::RemoveWorld(
        const String& name)
    {
        auto iter = m_Worlds.find(name);
        if (iter != m_Worlds.end())
        {
            if (&iter->second == m_ActiveWorld)
            {
                m_ActiveWorld = nullptr;
            }
            m_Worlds.erase(iter);
        }
    }

    bool Universe::HasWorld(
        const String& name)
    {
        return m_Worlds.contains(name);
    }

    World& Universe::GetWorld(
        const String& name)
    {
        return m_Worlds.at(name);
    }

    //

    void Universe::SetActiveWorld(
        World* world)
    {
#ifdef AME_DEBUG
        if (world)
        {
            for (auto& [name, curWorld] : m_Worlds)
            {
                if (&curWorld == world)
                {
                    InvokeChangeWorld(world);
                    return;
                }
            }
            world = nullptr;
            Log::Ecs().Error("Universe::SetActiveWorld: World not found");
        }
#endif
        InvokeChangeWorld(world);
    }

    bool Universe::HasActiveWorld() const
    {
        return m_ActiveWorld != nullptr;
    }

    const World* Universe::GetActiveWorld() const
    {
        return m_ActiveWorld;
    }

    World* Universe::GetActiveWorld()
    {
        return m_ActiveWorld;
    }

    //

    void Universe::ProgressActiveWorld(
        double deltaTime)
    {
        if (m_ActiveWorld)
        {
            m_ActiveWorld->Progress(deltaTime);
        }
    }

    void Universe::InvokeChangeWorld(
        World* newWorld)
    {
        if (m_ActiveWorld != newWorld)
        {
            auto OldWorld = m_ActiveWorld;
            m_ActiveWorld = newWorld;
            m_OnWorldChange({ OldWorld, newWorld });
        }
    }
} // namespace Ame::Ecs
