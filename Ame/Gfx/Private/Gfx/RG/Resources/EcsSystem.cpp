#include <Gfx/RG/Resources/EcsSystem.hpp>

namespace Ame::Gfx::RG
{
    EcsSystemHooks::EcsSystemHooks(
        Ecs::Universe& Universe) :
        m_Universe(Universe)
    {
        m_OnWorldChange = {
            m_Universe.get().OnWorldChange().ObjectSignal(),
            [this](auto& Universe, auto& ChangeData)
            {
                OnWorldChange();
            }
        };
    }

    EcsSystemHooks::EcsSystemHooks(
        EcsSystemHooks&& Other) :
        m_Universe(std::move(Other.m_Universe))
    {
    }

    EcsSystemHooks& EcsSystemHooks::operator=(
        EcsSystemHooks&& Other)
    {
        if (this != &Other)
        {
            Reset();
            m_Universe = std::move(Other.m_Universe);
        }
        return *this;
    }

    EcsSystemHooks::~EcsSystemHooks()
    {
        Reset();
    }

    //

    void EcsSystemHooks::Reset()
    {
    }

    void EcsSystemHooks::ApplyHooks()
    {
    }

    //

    void EcsSystemHooks::OnWorldChange()
    {
        Reset();
        ApplyHooks();
    }
} // namespace Ame::Gfx::RG