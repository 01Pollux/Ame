#pragma once

#include <Ecs/Universe.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources;

    class EcsSystemHooks
    {
    public:
        EcsSystemHooks(
            Ecs::Universe& Universe,
            CoreResources& Resources);

        EcsSystemHooks(const EcsSystemHooks&) = delete;
        EcsSystemHooks(EcsSystemHooks&& Other) noexcept;

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
        void ApplyTransformObserver();

    private:
        /// <summary>
        /// Called when the world changes.
        /// </summary>
        void OnWorldChange();

    private:
        Ref<Ecs::Universe> m_Universe;
        Ref<CoreResources> m_CoreResources;

        Signals::OnWorldChange::Handle m_OnWorldChange;

        Ecs::UniqueObserver m_TransformObserver;
    };
} // namespace Ame::Gfx::RG