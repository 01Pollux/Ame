#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

namespace Ame::Gfx::RG
{
    class EntityStore
    {
    public:
        struct Row
        {
            Rhi::Buffer                 VtxBuffer;
            Rhi::Buffer                 IdxBuffer;
            Ptr<Gfx::Shading::Material> Material;
            uint32_t                    Count = 1;

            Row(
                Rhi::Buffer                 Vtx,
                Rhi::Buffer                 Idx,
                Ptr<Gfx::Shading::Material> Material,
                uint32_t                    Count) :
                VtxBuffer(std::move(Vtx)),
                IdxBuffer(std::move(Idx)),
                Material(std::move(Material)),
                Count(Count)
            {
            }
        };

        struct CountedRow
        {
            Ref<const Row> CurRow;
            uint32_t       DrawOffset    = 0;
            uint32_t       CounterOffset = 0;

            auto operator->() const
            {
                return &CurRow.get();
            }
        };

        using RowGenerator        = Co::generator<Ref<const Row>>;
        using CountedRowGenerator = Co::generator<CountedRow>;

    public:
        EntityStore(
            RowGenerator Rows,
            uint32_t     EntityCount) :
            m_Rows(std::move(Rows)),
            m_EntityCount(EntityCount)
        {
        }

    public:
        [[nodiscard]] uint32_t GetEntitiesCount() const
        {
            return m_EntityCount;
        }

        /// <summary>
        /// Get the rows of the entity store.
        /// You can use this function or 'GetCountedRows' only once.
        /// </summary>
        [[nodiscard]] RowGenerator GetRows()
        {
            return std::move(m_Rows);
        }

        /// <summary>
        /// Get the rows of the entity store with the draw offset and the counter offset.
        /// You can use this function or 'GetRows' only once.
        /// </summary>
        [[nodiscard]] CountedRowGenerator GetCountedRows()
        {
            uint32_t DrawOffset    = 0;
            uint32_t CounterOffset = 0;
            uint32_t CommandOffset = 0;

            for (auto& Row : m_Rows)
            {
                co_yield CountedRow{
                    .CurRow        = Row,
                    .DrawOffset    = DrawOffset,
                    .CounterOffset = CounterOffset
                };

                DrawOffset += Row.get().Count;
                CounterOffset++;
            }
        }

    private:
        RowGenerator m_Rows;
        uint32_t     m_EntityCount;
    };
} // namespace Ame::Gfx::RG