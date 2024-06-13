#include <Gfx/Ecs/System.hpp>

namespace Ame::Gfx
{
    void EcsSystemHooks::CreateTransformObserver(
        Ecs::World& world)
    {
        auto transformObserverCallback =
            [this](Ecs::Iterator& iter, const Ecs::Component::Transform* transforms)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& gpuId = entity.GetComponentMut<TransformBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    if (gpuId.Id == m_TransformBuffer.c_InvalidSlot)
                    {
                        gpuId.Id = m_TransformBuffer.Rent();
                    }
                    auto matrix = transforms[i].ToMat4x4Transposed();
                    m_TransformBuffer.Write(gpuId.Id, std::bit_cast<const std::byte*>(matrix.data()), sizeof(matrix));
                }
                else
                {
                    if (gpuId.Id != m_TransformBuffer.c_InvalidSlot)
                    {
                        m_TransformBuffer.Return(gpuId.Id);
                        gpuId.Id = m_TransformBuffer.c_InvalidSlot;
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
} // namespace Ame::Gfx