#include <Ecs/Universe.hpp>
#include <Frame/Frame.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
    Universe::Universe(
        IFrame&     Frame,
        FrameTimer& Timer)
    {
        m_OnUpdate = {
            Frame.OnUpdate()
                .ObjectSignal(),
            [this, &Timer]
            { ProgressActiveWorld(Timer.GetDeltaTime()); }
        };
    }

    World& Universe::CreateWorld(
        const StringU8& Name)
    {
#ifdef AME_DEBUG
        if (m_Worlds.contains(Name))
        {
            Log::Ecs().Error("Universe::CreateWorld: World already exists");
            return m_Worlds.at(Name);
        }
#endif
        return m_Worlds.emplace(Name, World{ Name }).first->second;
    }

    void Universe::RemoveWorld(
        const StringU8& Name)
    {
        auto Iter = m_Worlds.find(Name);
        if (Iter != m_Worlds.end())
        {
            if (&Iter->second == m_ActiveWorld)
            {
                m_ActiveWorld = nullptr;
            }
            m_Worlds.erase(Iter);
        }
    }

    bool Universe::HasWorld(
        const StringU8& Name)
    {
        return m_Worlds.contains(Name);
    }

    World& Universe::GetWorld(
        const StringU8& Name)
    {
        return m_Worlds.at(Name);
    }

    //

    void Universe::SetActiveWorld(
        World& EcsWorld)
    {
#ifdef AME_DEBUG
        for (auto& [Name, World] : m_Worlds)
        {
            if (&World == &EcsWorld)
            {
                auto& OldWorld = m_ActiveWorld;
                m_ActiveWorld  = &EcsWorld;
                OnWorldChange().Broadcast(*this, { OldWorld, EcsWorld });
                return;
            }
        }
        Log::Ecs().Error("Universe::SetActiveWorld: World not found");
#else
        auto& OldWorld = m_ActiveWorld;
        m_ActiveWorld = &EcsWorld;
        OnWorldChange().Broadcast(*this, { OldWorld, EcsWorld });
#endif
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
        double DeltaTime)
    {
        if (m_ActiveWorld)
        {
            m_ActiveWorld->Progress(DeltaTime);
        }
    }
} // namespace Ame::Ecs
