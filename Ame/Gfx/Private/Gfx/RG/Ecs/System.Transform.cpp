#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace Ame::Gfx::RG
{
    void EcsSystemHooks::CreateTransformObserver()
    {
        auto& world = *m_Universe.get().GetActiveWorld();

        auto transformObserverCallback =
            [this](Ecs::Iterator& iter, const Ecs::Component::Transform* transforms)
        {
            auto& transformBuffer = m_CoreResources.get().GetTransformBuffer();
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& instanceInfo = entity.GetComponentMut<RenderInstance>();
                if (iter.event() == flecs::OnSet)
                {
                    if (instanceInfo.TransformIndex == transformBuffer.InvalidIndex)
                    {
                        instanceInfo.TransformIndex = transformBuffer.Rent();
                    }
                    auto matrix = transforms[i].ToMat4x4Transposed();
                    transformBuffer.Write(instanceInfo.TransformIndex, glm::value_ptr(matrix), sizeof(matrix));
                }
                else
                {
                    if (instanceInfo.TransformIndex != transformBuffer.InvalidIndex)
                    {
                        transformBuffer.Return(instanceInfo.TransformIndex);
                        instanceInfo.TransformIndex = transformBuffer.InvalidIndex;
                    }
                }
            }
        };

        m_WorldData.TransformObserver =
            world.CreateObserver<const Ecs::Component::Transform>()
                .with<const Ecs::Component::BaseRenderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(transformObserverCallback);
    }
} // namespace Ame::Gfx::RG