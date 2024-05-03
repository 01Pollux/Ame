#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

#include <Ecs/Component/Renderable/Renderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Gfx::RG
{
    void EcsSystemHooks::ApplyTransformObserver()
    {
        auto& World = *m_Universe.get().GetActiveWorld();

        auto TransformObserverCallback =
            [this](Ecs::Iterator& Iter, const Ecs::Component::Transform* Transforms)
        {
            auto& TransformBuffer = m_CoreResources.get().GetTransformBuffer();
            for (size_t i : Iter)
            {
                auto&       Transform = Transforms[i];
                Ecs::Entity Entity(Iter.entity(i));

                auto& InstanceInfo = Entity.GetComponentMut<Ecs::Gfx::Component::RenderInstance>();
                if (Iter.event() == flecs::OnSet)
                {
                    if (InstanceInfo.TransformIndex == TransformBuffer.InvalidIndex)
                    {
                        InstanceInfo.TransformIndex = TransformBuffer.Rent(Transform);
                    }
                    TransformBuffer.Write(InstanceInfo.TransformIndex, Transform);
                }
                else
                {
                    if (InstanceInfo.TransformIndex != TransformBuffer.InvalidIndex)
                    {
                        TransformBuffer.Return(InstanceInfo.TransformIndex);
                        InstanceInfo.TransformIndex = TransformBuffer.InvalidIndex;
                    }
                }
            }
        };

        m_WorldData->TransformObserver =
            World.CreateObserver<const Ecs::Component::Transform>()
                .with<const Ecs::Component::Renderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(TransformObserverCallback);
    }
} // namespace Ame::Gfx::RG