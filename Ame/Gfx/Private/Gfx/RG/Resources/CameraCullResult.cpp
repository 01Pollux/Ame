#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    auto CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        bool UniquerVertexa = Renderable.get().Vertex.HasUniqueBuffer();
        bool UniquerVertexb = Other.Renderable.get().Vertex.HasUniqueBuffer();

        bool UniqueIndexa = Renderable.get().Index.HasUniqueBuffer();
        bool UniqueIndexb = Other.Renderable.get().Index.HasUniqueBuffer();

        auto a = std::tie(Renderable.get().PipelineState, UniquerVertexa, UniqueIndexa, Distance);
        auto b = std::tie(Other.Renderable.get().PipelineState, UniquerVertexb, UniqueIndexb, Other.Distance);
        auto c = a <=> b;

        return c;
    }

    //

    CameraCullResult::CameraCullResult(
        Rhi::Device&          RhiDevice,
        const CameraCullDesc& Desc) :
        m_DynamicVertices(RhiDevice, Desc.VertexDesc),
        m_DynamicIndices(RhiDevice, Desc.IndexDesc),
        m_AllInstances(RhiDevice, Desc.InstanceDesc)
    {
        m_Rows.reserve(Desc.EstimatedRowSize);
        m_StagedEntities.reserve(Desc.EstimatedEntitiesCount);
    }

    void CameraCullResult::AddEntity(
        float                                 Distance,
        const Ecs::Component::BaseRenderable& Renderable,
        RenderInstance&                       Instance)
    {
        if (Renderable.Vertex.Count == 0 || Renderable.Index.Count == 0)
        {
			return;
		}

        m_StagedEntities.emplace(Renderable, Instance, Distance);
    }

    void CameraCullResult::Upload()
    {
        if (m_StagedEntities.empty())
        {
            return;
        }

        Reset();
        uint32_t LastVertexBlock = std::numeric_limits<uint32_t>::max(),
                 LastIndexBlock  = std::numeric_limits<uint32_t>::max();

        auto FetchBuffer = [](auto& DynamicBuffer, auto& View, uint32_t& LastBlock, bool& NewRow)
        {
            uint32_t    Offset;
            Rhi::Buffer Buffer;

            // If the vertex or index buffer is unique, create a new row
            if (View.HasUniqueBuffer())
            {
                NewRow = true;
                Offset = View.GetUniqueBufferOffset();
                Buffer = View.RhiBuffer;
            }
            else
            {
                auto Handle = DynamicBuffer.Rent(View.Count);
                Offset      = Handle.Offset;
                Buffer      = DynamicBuffer.GetBuffer(Handle.BlockSlot);
                // if the buffer is different, create a new row
                if (Handle.BlockSlot != LastBlock)
                {
                    NewRow    = true;
                    LastBlock = Handle.BlockSlot;
                }
            }

            return std::pair{ Buffer, Offset };
        };

        for (auto& [Renderable, Instance, Distance] : m_StagedEntities)
        {
            auto& Vertex = Renderable.get().Vertex;
            auto& Index  = Renderable.get().Index;

            bool NewRow = false;

            auto [VtxBuffer, VertexOffset] = FetchBuffer(m_DynamicVertices, Vertex, LastVertexBlock, NewRow);
            auto [IdxBuffer, IndexOffset]   = FetchBuffer(m_DynamicIndices, Index, LastIndexBlock, NewRow);

            Instance.get().VertexOffset = VertexOffset;
            Instance.get().VertexSize   = Vertex.Count;
            Instance.get().IndexOffset  = IndexOffset;
            Instance.get().IndexCount   = Index.Count;

            if (NewRow)
            {
                m_Rows.emplace_back(VtxBuffer, IdxBuffer, Renderable.get().PipelineState);
            }
            else
            {
                m_Rows.back().Count++;
            }

            m_AllInstances.Rent(Instance);
        }
        m_StagedEntities.clear();
    }

    void CameraCullResult::Reset()
    {
        m_Rows.clear();
        m_Rows.reserve(m_StagedEntities.size());
        // TODO: this should be camera based buffers
        m_AllInstances.Reset();
        m_DynamicIndices.Reset();
        m_DynamicVertices.Reset();
    }
} // namespace Ame::Gfx::RG