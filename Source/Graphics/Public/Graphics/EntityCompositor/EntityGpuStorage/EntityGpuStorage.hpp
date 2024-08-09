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
        } -> std::same_as<Ecs::ObserverBuilder<>>;
    };

    template<EntityGpuStorageTraits Traits>
    class EntityGpuStorage
    {
    public:
        using BuddyAllocator = Allocator::Buddy<Allocator::BuddyTraits_U32>;

        using traits_type = Traits;

        static constexpr uint32_t c_InvalidId = std::numeric_limits<uint32_t>::max();
        static constexpr uint32_t c_ChunkSize = 1024;

    public:
        EntityGpuStorage(
            Ecs::WorldRef world)
        {
            world->component<typename Traits::id_container_type>();
            m_Observer =
                Traits::observer_create(world)
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
        }

    private:
        /// <summary>
        /// Add or Update entity state.
        /// Function is not thread safe.
        /// </summary>
        void UpdateEntity(
            const Ecs::Entity& entity)
        {
            auto& instanceId = entity->ensure<typename Traits::id_container_type>();
            auto  newId      = UpdateEntity(entity.GetId(), instanceId.Id);
            if (instanceId.Id != newId)
            {
                instanceId.Id = newId;
                entity->modified<typename Traits::id_container_type>();
            }
        }

        void RemoveEntity(
            const Ecs::Entity& entity)
        {
            auto instanceId = entity->get<typename Traits::id_container_type>();
            if (instanceId)
            {
                RemoveEntity(instanceId->Id);
            }
        }

        /// <summary>
        /// Function is not thread safe.
        /// </summary>
        void RemoveEntity(
            uint32_t id)
        {
            m_Allocator.Free(BuddyAllocator::Handle{ id, 1 });
        }

    public:
        /// <summary>
        /// Updates the buffer with the dirty instances.
        /// </summary>
        void Upload(
            const Ecs::WorldRef& world,
            Dg::IRenderDevice*   renderDevice,
            Dg::IDeviceContext*  renderContext)
        {
            // At least 1 instance must be available
            size_t requiredSize = sizeof(typename Traits::instance_type) * GetMaxCount();
            TryGrowBuffer(renderDevice, renderContext, requiredSize);

            Dg::MapHelper<typename Traits::instance_type> bufferData(renderContext, m_Buffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);

            if (m_DirtyInstances.empty())
            {
                return;
            }
            for (auto id : m_DirtyInstances)
            {
                auto entity = world.GetEntityById(id);
                if (!entity)
                {
                    continue;
                }

                auto renderDataId = entity->get<typename Traits::id_container_type>();
                Traits::update(*entity, bufferData[renderDataId->Id]);
            }
            m_DirtyInstances.clear();
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
        /// Add or Update entity state.
        /// Function is not thread safe.
        /// </summary>
        [[nodiscard]] uint32_t UpdateEntity(
            Ecs::EntityId entityId,
            uint32_t      curId)
        {
            if (curId == c_InvalidId)
            {
                auto allocation = m_Allocator.Allocate(1);
                if (!allocation)
                {
                    m_Allocator.Grow(c_ChunkSize);
                    allocation = m_Allocator.Allocate(1);
                }
                curId = allocation.Offset;
            }

            m_DirtyInstances.push_back(entityId);
            return curId;
        }

        void TryGrowBuffer(
            Dg::IRenderDevice*  renderDevice,
            Dg::IDeviceContext* renderContext,
            size_t              newSize)
        {
            const Dg::BufferDesc* oldBufferDesc = nullptr;
            if (m_Buffer)
            {
                oldBufferDesc = &m_Buffer->GetDesc();
                if (oldBufferDesc->Size <= newSize)
                {
                    return;
                }
            }

            Dg::BufferDesc bufferDesc{
#ifndef AME_DIST
                Traits::name,
#else
                nullptr,
#endif
                newSize,
                Dg::BIND_SHADER_RESOURCE,
                Dg::USAGE_DYNAMIC,
                Dg::CPU_ACCESS_WRITE,
                Dg::BUFFER_MODE_STRUCTURED,
                sizeof(typename Traits::instance_type)
            };

            auto oldBuffer = std::move(m_Buffer);

            Dg::BufferData bufferData;
            if (oldBuffer)
            {
                Dg::PVoid mappedData = nullptr;
                renderContext->MapBuffer(oldBuffer, Dg::MAP_READ, Dg::MAP_FLAG_NONE, mappedData);
                bufferData.pData    = mappedData;
                bufferData.DataSize = oldBufferDesc->Size;
                bufferData.pContext = renderContext;
            }

            renderDevice->CreateBuffer(bufferDesc, oldBuffer ? &bufferData : nullptr, &m_Buffer);

            if (oldBuffer)
            {
                renderContext->UnmapBuffer(m_Buffer, Dg::MAP_READ);
            }
        }

    private:
        Ptr<Dg::IBuffer> m_Buffer;
        BuddyAllocator   m_Allocator{ c_ChunkSize };

        Ecs::UniqueObserver        m_Observer;
        std::vector<Ecs::EntityId> m_DirtyInstances;
    };
} // namespace Ame::Gfx