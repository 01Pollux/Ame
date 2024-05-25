#pragma once

#include <Ecs/Signals/Universe.hpp>
#include <Ecs/Universe.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources;
    struct RenderInstance;

    using CameraRenderRule = Ecs::UniqueRule<
        RenderInstance,
        const Ecs::Component::Transform,
        const Ecs::Component::BaseRenderable>;

    class EcsSystemHooks
    {
        struct EntityDesc
        {
            Ecs::UniqueObserver TransformObserver;
            CameraRenderRule    RenderRule;
        };

    public:
        EcsSystemHooks(
            Ecs::Universe& universe,
            CoreResources& coreResources);

    public:
        [[nodiscard]] CameraRenderRule& GetCameraRule() noexcept
        {
            return m_WorldData.RenderRule;
        }

    private:
        void CreateTransformObserver();
        void CreateCameraRule();

    private:
        Ref<Ecs::Universe> m_Universe;
        Ref<CoreResources> m_CoreResources;

        Signals::OnWorldChange::Handle m_OnWorldChange;
        EntityDesc                     m_WorldData;
    };
} // namespace Ame::Gfx::RG