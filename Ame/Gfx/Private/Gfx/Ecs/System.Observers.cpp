#include <Gfx/Ecs/System.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Math/AABB.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx
{
    void EcsSystemHooks::CreateObservers(
        Ecs::World& world)
    {
        auto transformCallback =
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

        auto aabbCallback =
            [this](Ecs::Iterator& iter, const Ecs::Component::AABB* aabbs)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& gpuId = entity.GetComponentMut<AABBBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    if (gpuId.Id == m_AABBBuffer.c_InvalidSlot)
                    {
                        gpuId.Id = m_AABBBuffer.Rent();
                    }
                    AlignedAABB aabb{ aabbs[i].ToAABB() };
                    m_AABBBuffer.Write(gpuId.Id, aabb);
                }
                else
                {
                    if (gpuId.Id != m_AABBBuffer.c_InvalidSlot)
                    {
                        m_AABBBuffer.Return(gpuId.Id);
                        gpuId.Id = m_AABBBuffer.c_InvalidSlot;
                    }
                }
            }
        };

        m_WorldData.TransformObserver =
            world.CreateObserver<const Ecs::Component::Transform>()
                .with<const Ecs::Component::BaseRenderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(transformCallback);

        m_WorldData.AABBObserver =
            world.CreateObserver<const Ecs::Component::AABB>()
                .with<const Ecs::Component::BaseRenderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(aabbCallback);
    }
} // namespace Ame::Gfx