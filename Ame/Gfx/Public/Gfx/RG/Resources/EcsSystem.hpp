#pragma once

#include <Ecs/Universe.hpp>

namespace Ame::Gfx::RG
{
    class EcsSystemHooks
    {
    public:
        EcsSystemHooks(
            Ecs::Universe& Universe);

        EcsSystemHooks(const EcsSystemHooks&) = delete;
        EcsSystemHooks(EcsSystemHooks&& Other);

        EcsSystemHooks& operator=(const EcsSystemHooks&) = delete;
        EcsSystemHooks& operator=(EcsSystemHooks&& Other);

        ~EcsSystemHooks();

    private:
        /// <summary>
        /// Remove all hooks.
        /// </summary>
        void Reset();

        /// <summary>
        /// Apply all hooks for the current world.
        /// </summary>
        void ApplyHooks();

    private:
        /// <summary>
        /// Called when the world changes.
        /// </summary>
        void OnWorldChange();

    private:
        Ref<Ecs::Universe> m_Universe;

        Signals::OnWorldChange::Handle m_OnWorldChange;
    };
} // namespace Ame::Gfx::RG