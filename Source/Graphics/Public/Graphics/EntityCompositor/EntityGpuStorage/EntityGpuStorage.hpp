#pragma once

#include <Graphics/EntityCompositor/Core.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>
#include <Allocator/Utils/Buddy.hpp>

namespace Ame::Gfx
{
    template<typename T>
    concept EntityGpuStorageTraits = requires(const Ecs::Entity& entity) {
        {
            T::name
        } -> std::convertible_to<const char*>;
        typename T::id_container_type;
        typename T::instance_type;
        {
            T::update(entity, std::declval<typename T::instance_type&>())
        };
        {
            T::observer_create(std::declval<Ecs::WorldRef>())
        };
    };

    template<EntityGpuStorageTraits Traits>
    class EntityGpuStorage
    {
    public:
        using traits_type            = Traits;
        using buddy_allocator_type   = Allocator::Buddy<Allocator::BuddyTraits_U32>;
        using instance_observer_type = Ecs::UniqueQuery<const typename traits_type::id_container_type>;

        static constexpr uint32_t c_InvalidId = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t c_ChunkSize = 1024;

    private:
        struct UnsortedEntityId
        {
            Ecs::EntityId EntityId;
            uint32_t      InstanceId;
        };

        struct SortedIdRange
        {
            uint32_t FirstId;
            uint32_t Count;
        };

        struct SortedIdRangeResult
        {
            std::vector<typename traits_type::instance_type> Instances;
            std::vector<SortedIdRange>                       Ranges;
            uint32_t                                         LargestCount = 0;
        };

    public:
        EntityGpuStorage(
            Ecs::WorldRef world)
        {
            world->component<typename traits_type::id_container_type>();
            m_Observer =
                traits_type::observer_create(world)
                    .yield_existing()
                    .run(
                        [this](Ecs::Iterator& iter)
                        {
                            while (iter.next())
                            {
                                for (size_t i : iter)
                                {
                                    Ecs::Entity entity(iter.entity(i));
                                    if (iter.event() == flecs::OnSet)
                                    {
                                        UpdateEntity(entity);
                                    }
                                    else
                                    {
                                        RemoveEntity(entity);
                                    }
                                }
                            }
                        });
            m_InstanceObserver =
                world->query_builder<const typename traits_type::id_container_type>()
                    .cached()
                    .build();
        }

    private:
        /// <summary>
        /// Add or Update entity state.
        /// Function is not thread safe.
        /// </summary>
        void UpdateEntity(
            const Ecs::Entity& entity)
        {
            auto& instanceId = entity->ensure<typename traits_type::id_container_type>();
            if (instanceId.Id == c_InvalidId)
            {
                instanceId.Id = AllocateId();
                entity->modified<typename traits_type::id_container_type>();
            }
        }

        void RemoveEntity(
            const Ecs::Entity& entity)
        {
            auto instanceId = entity->get<typename traits_type::id_container_type>();
            if (instanceId)
            {
                m_Allocator.Free(buddy_allocator_type::Handle{ instanceId->Id, 1 });
                entity->remove<typename traits_type::id_container_type>();
            }
        }

    public:
        /// <summary>
        /// Updates the buffer with the dirty instances.
        /// </summary>
        void Upload(
            Dg::IRenderDevice*  renderDevice,
            Dg::IDeviceContext* renderContext)
        {
            // At least 1 instance must be available
            size_t requiredSize      = sizeof(typename traits_type::instance_type) * GetMaxCount();
            bool   wholeBufferUpdate = TryGrowBuffer(renderDevice, requiredSize);

            if (!wholeBufferUpdate && !m_InstanceObserver->changed())
            {
                return;
            }

            m_InstanceObserver->run(
                [&](Ecs::Iterator& iter)
                {
                    std::vector<UnsortedEntityId> collectedIds;

                    while (iter.next())
                    {
                        if (!wholeBufferUpdate && !iter.changed())
                        {
                            continue;
                        }

                        auto ids = iter.field<const typename traits_type::id_container_type>(0);
                        collectedIds.reserve(collectedIds.size() + iter.count());

                        for (size_t i : iter)
                        {
                            collectedIds.emplace_back(iter.entity(i), ids[i].Id);
                        }
                    }

                    if (!collectedIds.empty())
                    {
                        auto result = GroupAndSortIds(iter, collectedIds);

                        uint32_t bufferOffset = 0;
                        for (auto& [start, count] : result.Ranges)
                        {
                            renderContext->UpdateBuffer(
                                m_Buffer,
                                start * sizeof(typename traits_type::instance_type),
                                count * sizeof(typename traits_type::instance_type),
                                result.Instances.data() + bufferOffset,
                                Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                            bufferOffset += count;
                        }
                    }
                });
        }

    public:
        [[nodiscard]] size_t GetMaxCount() const noexcept
        {
            return std::max(m_Allocator.GetMaxSize(), 1u);
        }

        [[nodiscard]] Ptr<Dg::IBuffer> GetBuffer() const noexcept
        {
            return m_Buffer;
        }

    private:
        /// <summary>
        /// Give a list of ids, those ids may or may not be sorted, and their values may or may not be contigious.
        /// We will transform them into a list of contigious sorted ids.
        /// [5, 1, 2, 6, 9, 0, 3]
        /// 1: [0, 3] (0, 1, 2, 3)
        /// 2: [5, 2] (5, 6)
        /// 3: [9, 1] (9)
        /// </summary>
        [[nodiscard]] SortedIdRangeResult GroupAndSortIds(
            Ecs::Iterator&              iter,
            std::span<UnsortedEntityId> ids)
        {
            SortedIdRangeResult result;

            std::sort(ids.begin(), ids.end(), [](const auto& a, const auto& b)
                      { return a.InstanceId < b.InstanceId; });
            result.Instances.resize(ids.size());

            auto     start  = ids[0].InstanceId;
            auto     prevId = start;
            uint32_t count  = 1;

            traits_type::update(flecs::entity{ iter.world(), ids[0].EntityId }, result.Instances[0]);
            for (size_t i = 1; i < ids.size(); i++)
            {
                auto currentId = ids[i].InstanceId;

                traits_type::update(flecs::entity{ iter.world(), ids[i].EntityId }, result.Instances[i]);
                if (currentId - prevId > 1)
                {
                    result.Ranges.emplace_back(start, count);
                    result.LargestCount = std::max(result.LargestCount, count);

                    start = currentId;
                    count = 0;
                }

                prevId = currentId;
                count++;
            }

            result.Ranges.emplace_back(start, count);
            result.LargestCount = std::max(result.LargestCount, count);

            return result;
        }

    private:
        /// <summary>
        /// Add or Update entity state.
        /// Function is not thread safe.
        /// </summary>
        [[nodiscard]] uint32_t AllocateId()
        {
            auto allocation = m_Allocator.Allocate(1);
            if (!allocation)
            {
                m_Allocator.Grow(c_ChunkSize);
                allocation = m_Allocator.Allocate(1);
            }
            return allocation.Offset;
        }

        /// <summary>
        /// Try to grow the buffer to the new size.
        /// Returns true if the buffer was grown, this is used to avoid whole buffer update.
        /// </summary>
        [[nodiscard]] bool TryGrowBuffer(
            Dg::IRenderDevice* renderDevice,
            size_t             newSize)
        {
            const Dg::BufferDesc* oldBufferDesc = nullptr;
            if (m_Buffer)
            {
                oldBufferDesc = &m_Buffer->GetDesc();
                if (oldBufferDesc->Size <= newSize)
                {
                    return false;
                }
            }

            Dg::BufferDesc bufferDesc{
#ifndef AME_DIST
                traits_type::name,
#else
                nullptr,
#endif
                newSize,
                Dg::BIND_SHADER_RESOURCE,
                Dg::USAGE_DEFAULT,
                Dg::CPU_ACCESS_NONE,
                Dg::BUFFER_MODE_STRUCTURED,
                sizeof(typename traits_type::instance_type)
            };

            renderDevice->CreateBuffer(bufferDesc, nullptr, &m_Buffer);
            return true;
        }

    private:
        Ptr<Dg::IBuffer>     m_Buffer;
        buddy_allocator_type m_Allocator{ c_ChunkSize };

        Ecs::UniqueObserver    m_Observer;
        instance_observer_type m_InstanceObserver;
    };
} // namespace Ame::Gfx