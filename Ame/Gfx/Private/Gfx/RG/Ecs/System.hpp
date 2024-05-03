#pragma once

#include <Ecs/Signals/Universe.OnWorldChangeHelper.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources;

    class EcsSystemHooks
    {
        struct EntityDesc
        {
            Ecs::UniqueObserver TransformObserver;
        };

    public:
        EcsSystemHooks(
            Ecs::Universe& Universe,
            CoreResources& Resources);

    private:
        /// <summary>
        /// Apply all hooks for the current world.
        /// </summary>
        void ApplyHooks();

    private:
        void ApplyTransformObserver();

    private:
        Ref<Ecs::Universe> m_Universe;
        Ref<CoreResources> m_CoreResources;

        Signals::OnWorldChangeHelper<EntityDesc> m_WorldData;
    };
} // namespace Ame::Gfx::RG