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
        auto getOrAllocateFromBuffer = [](auto& buffer, auto entity, auto curGpuId, auto invalidId)
        {
            auto gpuId{ curGpuId ? *curGpuId : invalidId };
            if (!curGpuId)
            {
                gpuId.Id = buffer.Rent();
                entity->set(gpuId);
            }
            return gpuId;
        };

        auto removeFromBuffer = [](auto& buffer, auto entity, auto curGpuId)
        {
            if (curGpuId)
            {
                buffer.Return(curGpuId->Id);
                entity->remove<decltype(curGpuId)>();
            }
        };

        auto getOrInvalid = [](auto gpuId, auto invalidId)
        {
            return gpuId ? gpuId->Id : invalidId;
        };

        //

        auto transformCallback =
            [&](Ecs::Iterator&                   iter,
                const Ecs::Component::Transform* transforms)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto transformIdComponent = entity->get<TransformBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    auto transformId = getOrAllocateFromBuffer(m_TransformBuffer, entity, transformIdComponent, TransformBuffer::GpuId{});
                    auto matrix      = transforms[i].ToMat4x4Transposed();
                    m_TransformBuffer.Write(transformId.Id, std::bit_cast<const std::byte*>(matrix.data()), sizeof(matrix));
                }
                else
                {
                    removeFromBuffer(m_TransformBuffer, entity, transformIdComponent);
                }
            }
        };

        auto aabbCallback =
            [&](Ecs::Iterator&              iter,
                const Ecs::Component::AABB* aabbs)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto aabbIdComponent = entity->get<AABBBuffer::GpuId>();
                if (iter.event() == flecs::OnSet)
                {
                    auto aabbId = getOrAllocateFromBuffer(m_AABBBuffer, entity, aabbIdComponent, AABBBuffer::GpuId{});
                    auto box    = aabbs[i].ToAABB();
                    m_AABBBuffer.Write(aabbId.Id, std::bit_cast<const std::byte*>(&box), sizeof(box));
                }
                else
                {
                    removeFromBuffer(m_AABBBuffer, entity, aabbIdComponent);
                }
            }
        };

        auto instanceCallback =
            [&](Ecs::Iterator&                iter,
                const TransformBuffer::GpuId* transformIds,
                const AABBBuffer::GpuId*      aabbIds)
        {
            for (size_t i : iter)
            {
                Ecs::Entity entity(iter.entity(i));
                if (iter.event() == flecs::OnSet)
                {
                    RenderInstance instance{
                        .TransformId = transformIds[i].Id,
                        .AABBId      = aabbIds[i].Id
                    };

                    auto instanceIdComponent = entity->get<InstanceBuffer::GpuId>();
                    auto instanceId          = getOrAllocateFromBuffer(m_InstanceBuffer, entity, instanceIdComponent, InstanceBuffer::GpuId{});
                    m_InstanceBuffer.Write(instanceId.Id, std::bit_cast<const std::byte*>(&instance), sizeof(instance));
                    m_InstanceBuffer.Flush();
                }
                else
                {
                    removeFromBuffer(m_InstanceBuffer, entity, entity->get<InstanceBuffer::GpuId>());
                }
            }
        };

        //

        m_WorldData.RenderRule =
            world.CreateRule<const RenderInstance::GpuId,
                             const Ecs::Component::Transform,
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

        m_WorldData.InstanceObserver =
            world.CreateObserver<const TransformBuffer::GpuId,
                                 const AABBBuffer::GpuId>()
                .with<const Ecs::Component::BaseRenderable>()
                .event(flecs::OnSet)
                .event(flecs::OnRemove)
                .iter(instanceCallback);
    }
} // namespace Ame::Gfx