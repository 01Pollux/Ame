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

    Universe::Universe(Universe&& Other) :
        m_OnUpdate(std::move(Other.m_OnUpdate)),
        m_Worlds(std::move(Other.m_Worlds)),
        m_ActiveWorld(std::exchange(Other.m_ActiveWorld, nullptr))
    {
    }

    Universe& Universe::operator=(
        Universe&& Other)
    {
        if (this != &Other)
        {
            InvokeChangeWorld(nullptr);
            m_OnUpdate    = std::move(Other.m_OnUpdate);
            m_Worlds      = std::move(Other.m_Worlds);
            m_ActiveWorld = std::exchange(Other.m_ActiveWorld, nullptr);
        }
        return *this;
    }

    Universe::~Universe()
    {
        InvokeChangeWorld(nullptr);
    }

    //

    World& Universe::CreateWorld(
        const String& Name)
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
        const String& Name)
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
        const String& Name)
    {
        return m_Worlds.contains(Name);
    }

    World& Universe::GetWorld(
        const String& Name)
    {
        return m_Worlds.at(Name);
    }

    //

    void Universe::SetActiveWorld(
        World* EcsWorld)
    {
#ifdef AME_DEBUG
        if (EcsWorld)
        {
            for (auto& [Name, World] : m_Worlds)
            {
                if (&World == EcsWorld)
                {
                    InvokeChangeWorld(EcsWorld);
                    return;
                }
            }
            EcsWorld = nullptr;
            Log::Ecs().Error("Universe::SetActiveWorld: World not found");
        }
#endif
        InvokeChangeWorld(EcsWorld);
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
        double DeltaTime)
    {
        if (m_ActiveWorld)
        {
            m_ActiveWorld->Progress(DeltaTime);
        }
    }

    void Universe::InvokeChangeWorld(
        World* NewWorld)
    {
        if (m_ActiveWorld != NewWorld)
        {
            m_ActiveWorld = NewWorld;
            OnWorldChange().Broadcast(*this, { m_ActiveWorld, NewWorld });
        }
    }
} // namespace Ame::Ecs
