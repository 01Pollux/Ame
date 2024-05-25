#include <Gfx/RG/Resources/CameraCullResult.hpp>
#include <Gfx/Shading/Material.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    CameraCullResult::CameraCullResult(
        Rhi::Device&          rhiDevice,
        const CameraCullDesc& desc) :
        m_Device(rhiDevice),
        m_CameraVertexDesc(desc.VertexDesc),
        m_CameraIndexDesc(desc.IndexDesc),
        m_CameraInstanceDesc(desc.InstanceDesc)
    {
        m_Rows.reserve(desc.EstimatedRowSize);
        m_StagedEntities.reserve(desc.EstimatedEntitiesCount);
    }

    //

    uint32_t CameraCullResult::GetEntitiesCount() const
    {
        return m_EntitiesCount;
    }

    EntityStore::RowGenerator CameraCullResult::GetEntities() const
    {
        for (auto& row : m_Rows)
        {
            co_yield row;
        }
    }

    const InstanceBuffer& CameraCullResult::GetInstancesTableBuffer() const
    {
        Log::Gfx().Assert(!m_Cameras.empty(), "CameraCullResult::GetInstancesTableBuffer: No camera data available");
        return m_Cameras[m_CurrentCamera].AllInstances;
    }

    InstanceBuffer& CameraCullResult::GetInstancesTableBuffer()
    {
        Log::Gfx().Assert(!m_Cameras.empty(), "CameraCullResult::GetInstancesTableBuffer: No camera data available");
        return m_Cameras[m_CurrentCamera].AllInstances;
    }

    //

    void CameraCullResult::Reset()
    {
        m_CurrentCamera = -1;
    }

    void CameraCullResult::AddEntity(
        float                                 distance,
        const Ecs::Component::BaseRenderable& renderable,
        RenderInstance&                       instance)
    {
        if (renderable.Vertex.Count == 0 || renderable.Index.Count == 0) [[unlikely]]
        {
            return;
        }

        m_StagedEntities.emplace(renderable, instance, distance);
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
        uint32_t lastVertexBlock = std::numeric_limits<uint32_t>::max(),
                 lastIndexBlock  = std::numeric_limits<uint32_t>::max();

        auto& cameraStorage = m_Cameras[m_CurrentCamera];

        auto fetchBuffer = [](auto& dynamicBuffer, auto& view, uint32_t& lastBlock, bool& newRow)
        {
            nri::Buffer* buffer = nullptr;
            uint32_t     offset = 0;

            // If the vertex or index buffer is unique, create a new row
            // (1)
            if (view.HasUniqueBuffer())
            {
                newRow = true;
                offset = view.Offset();
                buffer = view.NriBuffer;
            }
            else
            {
                auto Handle = dynamicBuffer.Rent(view.CpuData(), view.Stride * view.Count);
                offset      = Handle.Offset;
                buffer      = dynamicBuffer.GetBuffer(Handle.BlockSlot).Unwrap();

                // if the buffer is different, create a new row
                // (2)
                if (Handle.BlockSlot != lastBlock)
                {
                    newRow    = true;
                    lastBlock = Handle.BlockSlot;
                }
            }

            return std::pair{ buffer, offset };
        };

        auto firstIter = m_StagedEntities.begin();
        auto lastIter  = firstIter;

        nri::Buffer* lastVertexBuffer = nullptr;
        nri::Buffer* lastIndexBuffer  = nullptr;

        Shading::Material* lastMaterial = nullptr;

        Rhi::IndexType lastIndexType = Rhi::IndexType::MAX_NUM;

        //

        // for each entity in m_StagedEntities
        //      mutate 'Instance' member to allocate vertex/index buffer if needed
        //      if one these are true, we will need to create new row:
        //          - (1) the vertex/index buffer is owning (unique and not dynamic)
        //          - (2) the previous vertex/index buffer is different than the current one
        //          - (3) the previous index buffer type is different than the current one
        //          - (4) the previous material's pipeline state is different than the current one
        for (auto& [Renderable, Instance, Distance] : m_StagedEntities)
        {
            auto& vertex = Renderable.get().Vertex;
            auto& index  = Renderable.get().Index;

            bool needsNewRow = false;

            //

            // (1) + (2)
            std::tie(lastVertexBuffer, Instance.get().VertexOffset) = fetchBuffer(cameraStorage.DynamicVertices, vertex, lastVertexBlock, needsNewRow);
            std::tie(lastIndexBuffer, Instance.get().IndexOffset) = fetchBuffer(cameraStorage.DynamicIndices, index, lastIndexBlock, needsNewRow);

            //

            // (3)
            auto indexType = index.Stride == sizeof(uint16_t) ? Rhi::IndexType::UINT16 : Rhi::IndexType::UINT32;
            if (lastIndexType != Rhi::IndexType::MAX_NUM && indexType == lastIndexType)
            {
                needsNewRow = true;
            }
            lastIndexType = indexType;

            //

            // (4)
            if (!lastMaterial ||
                Renderable.get().Material->GetPipelineHash() != lastMaterial->GetPipelineHash())
            {
                needsNewRow  = true;
                lastMaterial = Renderable.get().Material.get();
            }

            //

            Instance.get().VertexSize = vertex.Count * vertex.Stride;
            Instance.get().IndexCount = index.Count;

            cameraStorage.AllInstances.Rent(Instance);

            if (needsNewRow)
            {
                if (firstIter != lastIter)
                {
                    m_StagedGroups.emplace(std::span{ firstIter, lastIter }, lastVertexBuffer, lastIndexBuffer);
                    firstIter = lastIter;
                }
            }
            lastIter++;
        }

        if (firstIter != lastIter)
        {
            m_StagedGroups.emplace(std::span{ firstIter, lastIter }, lastVertexBuffer, lastIndexBuffer);
        }
    }

    void CameraCullResult::FinalizeStaging()
    {
        m_Cameras[m_CurrentCamera].Flush();

        for (auto& group : m_StagedGroups)
        {
            uint32_t count = static_cast<uint32_t>(group.Entities.size());
            m_EntitiesCount += count;
            m_Rows.emplace_back(
                std::move(group.VtxBuffer),
                std::move(group.IdxBuffer),
                group.GetIndexType(),
                group.GetMaterial(),
                count);
        }

        m_StagedGroups.clear();
        m_StagedEntities.clear();
    }
} // namespace Ame::Gfx::RG