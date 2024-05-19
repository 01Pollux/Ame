#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

#include <Log/Wrapper.hpp>

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

    //

    uint32_t CameraCullResult::GetEntitiesCount() const
    {
        return m_EntitiesCount;
    }

    EntityStore::RowGenerator CameraCullResult::GetEntities() const
    {
        for (auto& Row : m_Rows)
        {
            co_yield Row;
        }
    }

    const InstanceBuffer& CameraCullResult::GetInstancesTableBuffer() const
    {
        Log::Renderer().Assert(!m_Cameras.empty(), "CameraCullResult::GetInstancesTableBuffer: No camera data available");
        return m_Cameras[m_CurrentCamera].AllInstances;
    }

    //

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
        m_Rows.clear();
        m_EntitiesCount = 0;

        if (m_StagedEntities.empty())
        {
            return;
        }

        PrepareForUpload();
        StageUpload();
        FinalizeStaging();
    }

    //

    void CameraCullResult::PrepareForUpload()
    {
        m_Rows.reserve(m_StagedEntities.size());

        // Create a new camera storage if needed
        m_CurrentCamera++;
        if (m_CurrentCamera >= m_Cameras.size())
        {
            m_Cameras.emplace_back(m_Device.get(), m_CameraVertexDesc, m_CameraIndexDesc, m_CameraInstanceDesc);
        }
        m_Cameras[m_CurrentCamera].Reset();
    }

    void CameraCullResult::StageUpload()
    {
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

        auto First = m_StagedEntities.begin();
        auto Last  = First;

        Rhi::Buffer VtxBuffer;
        Rhi::Buffer IdxBuffer;

        Gfx::Shading::Material* LastMaterial = nullptr;

        for (auto& [Renderable, Instance, Distance] : m_StagedEntities)
        {
            auto& Vertex = Renderable.get().Vertex;
            auto& Index  = Renderable.get().Index;

            bool NewRow = false;

            std::tie(VtxBuffer, Instance.get().VertexOffset) = FetchBuffer(Storage.DynamicVertices, Vertex, LastVertexBlock, NewRow);
            std::tie(IdxBuffer, Instance.get().IndexOffset)  = FetchBuffer(Storage.DynamicIndices, Index, LastIndexBlock, NewRow);

            Instance.get().VertexSize = Vertex.Count;
            Instance.get().IndexCount = Index.Count;

            Storage.AllInstances.Rent(Instance);

            if (Renderable.get().Material.get() != LastMaterial)
            {
                NewRow       = true;
                LastMaterial = Renderable.get().Material.get();
            }

            if (NewRow)
            {
                if (First != Last)
                {
                    m_StagedGroups.emplace(std::span{ First, Last }, VtxBuffer, IdxBuffer);
                    First = Last;
                }
            }
            Last++;
        }

        if (First != Last)
        {
            m_StagedGroups.emplace(std::span{ First, Last }, VtxBuffer, IdxBuffer);
        }
    }

    void CameraCullResult::FinalizeStaging()
    {
        for (auto& Group : m_StagedGroups)
        {
            auto Count = static_cast<uint32_t>(Group.Entities.size());
            m_EntitiesCount += Count;
            m_Rows.emplace_back(
                std::move(Group.VtxBuffer),
                std::move(Group.IdxBuffer),
                Group.GetFirstRenderable().Material,
                Count);
        }

        m_StagedGroups.clear();
        m_StagedEntities.clear();
    }
} // namespace Ame::Gfx::RG