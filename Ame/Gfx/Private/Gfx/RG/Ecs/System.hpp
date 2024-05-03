#pragma once

#include <Ecs/Signals/Universe.OnWorldChangeHelper.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources;

    using CameraRenderRule = Ecs::UniqueRule<
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
            Ecs::Universe& Universe,
            CoreResources& Resources);

    public:
        [[nodiscard]] CameraRenderRule& GetCameraRule() noexcept
        {
            return m_WorldData->RenderRule;
        }

    private:
        void CreateTransformObserver();
        void CreateCameraRule();

    private:
        Ref<Ecs::Universe> m_Universe;
        Ref<CoreResources> m_CoreResources;

        Signals::OnWorldChangeHelper<EntityDesc> m_WorldData;
    };
} // namespace Ame::Gfx::RG