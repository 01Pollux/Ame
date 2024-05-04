#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    CameraCullResult::CameraCullResult(
        Rhi::Device&          RhiDevice,
        const CameraCullDesc& Desc) :
        m_Device(RhiDevice),
        m_CameraVertexDesc(Desc.VertexDesc),
        m_CameraIndexDesc(Desc.IndexDesc),
        m_CameraInstanceDesc(Desc.InstanceDesc)
    {
        m_Rows.reserve(Desc.EstimatedRowSize);
        m_StagedEntities.reserve(Desc.EstimatedEntitiesCount);
    }

    void CameraCullResult::Reset()
    {
        m_CurrentCamera = -1;
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

        PrepareForUpload();
        uint32_t LastVertexBlock = std::numeric_limits<uint32_t>::max(),
                 LastIndexBlock  = std::numeric_limits<uint32_t>::max();

        auto& Storage = m_Cameras[m_CurrentCamera];

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

            auto [VtxBuffer, VertexOffset] = FetchBuffer(Storage.DynamicVertices, Vertex, LastVertexBlock, NewRow);
            auto [IdxBuffer, IndexOffset]  = FetchBuffer(Storage.DynamicIndices, Index, LastIndexBlock, NewRow);

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

            Storage.AllInstances.Rent(Instance);
        }
        m_StagedEntities.clear();
    }

    void CameraCullResult::PrepareForUpload()
    {
        m_Rows.clear();
        m_Rows.reserve(m_StagedEntities.size());

        m_CurrentCamera++;
        if (m_CurrentCamera >= m_Cameras.size())
        {
            m_Cameras.emplace_back(m_Device.get(), m_CameraVertexDesc, m_CameraIndexDesc, m_CameraInstanceDesc);
        }
        m_Cameras[m_CurrentCamera].Reset();
    }
} // namespace Ame::Gfx::RG