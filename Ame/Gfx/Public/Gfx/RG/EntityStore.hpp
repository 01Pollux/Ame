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
            Ptr<Shading::Material> Material;

            nri::Buffer* VtxBuffer;
            nri::Buffer* IdxBuffer;

            uint32_t       Count = 1;
            Rhi::IndexType IndexType;

            Row(
                nri::Buffer*           vertexBuffer,
                nri::Buffer*           indexBuffer,
                Rhi::IndexType         indexType,
                Ptr<Shading::Material> material,
                uint32_t               count) :
                Material(std::move(material)),
                VtxBuffer(vertexBuffer),
                IdxBuffer(indexBuffer),
                Count(count),
                IndexType(indexType)
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
            RowGenerator rows,
            uint32_t     entityCount) :
            m_Rows(std::move(rows)),
            m_EntityCount(entityCount)
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

            for (auto& row : m_Rows)
            {
                co_yield CountedRow{
                    .CurRow        = row,
                    .DrawOffset    = DrawOffset,
                    .CounterOffset = CounterOffset
                };

                DrawOffset += row.get().Count;
                CounterOffset++;
            }
        }

    private:
        RowGenerator m_Rows;
        uint32_t     m_EntityCount;
    };
} // namespace Ame::Gfx::RG