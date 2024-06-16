#include <Gfx/Ecs/World.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Math/AABB.hpp>
#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Tag/VisibleToCamera.hpp>

namespace Ame::Gfx
{
    void EcsWorldResources::CreateObserversAndRules(
        Ecs::World& world)
    {
        auto transformCallback =
            [this](Ecs::Iterator& iter, const Ecs::Component::Transform* transforms)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto curGpuId = entity->get<TransformBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    TransformBuffer::GpuId gpuId{ curGpuId ? curGpuId->Id : m_TransformBuffer.c_InvalidSlot };
                    if (!curGpuId)
                    {
                        gpuId.Id = m_TransformBuffer.Rent();
                        entity->set(gpuId);
                    }
                    auto matrix = transforms[i].ToMat4x4Transposed();
                    m_TransformBuffer.Write(gpuId.Id, std::bit_cast<const std::byte*>(matrix.data()), sizeof(matrix));
                }
                else
                {
                    if (curGpuId)
                    {
                        m_TransformBuffer.Return(curGpuId->Id);
                        entity->remove<TransformBuffer::GpuId>();
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

                auto curGpuId = entity->get<AABBBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    AABBBuffer::GpuId gpuId{ curGpuId ? curGpuId->Id : m_AABBBuffer.c_InvalidSlot };
                    if (!curGpuId)
                    {
                        gpuId.Id = m_AABBBuffer.Rent();
                        entity->set(gpuId);
                    }
                    AlignedAABB aabb{ aabbs[i].ToAABB() };
                    m_AABBBuffer.Write(gpuId.Id, aabb);
                }
                else
                {
                    if (curGpuId)
                    {
                        m_AABBBuffer.Return(curGpuId->Id);
                        entity->remove<AABBBuffer::GpuId>();
                    }
                }
            }
        };

        //

        m_WorldData.RenderRule =
            world.CreateRule<const Ecs::Component::Transform,
                             const Ecs::Component::BaseRenderable>()
                .with<Ecs::Tag::VisibleToCamera>()
                .second("$Camera")
                .build();

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