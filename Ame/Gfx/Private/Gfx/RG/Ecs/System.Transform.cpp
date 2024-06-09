#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

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

                auto& gpuId = entity.GetComponentMut<TransformBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    if (gpuId.Id == transformBuffer.c_InvalidSlot)
                    {
                        gpuId.Id = transformBuffer.Rent();
                    }
                    auto matrix = transforms[i].ToMat4x4Transposed();
                    transformBuffer.Write(gpuId.Id, std::bit_cast<const std::byte*>(matrix.data()), sizeof(matrix));
                }
                else
                {
                    if (gpuId.Id != transformBuffer.c_InvalidSlot)
                    {
                        transformBuffer.Return(gpuId.Id);
                        gpuId.Id = transformBuffer.c_InvalidSlot;
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