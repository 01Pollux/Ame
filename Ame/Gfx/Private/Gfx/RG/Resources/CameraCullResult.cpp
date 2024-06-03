#include <execution>

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

    const DynamicInstanceBuffer& CameraCullResult::GetInstancesTableBuffer() const
    {
        Log::Gfx().Assert(!m_Cameras.empty(), "CameraCullResult::GetInstancesTableBuffer: No camera data available");
        return m_Cameras[m_CurrentCamera].AllInstances;
    }

    DynamicInstanceBuffer& CameraCullResult::GetInstancesTableBuffer()
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
        auto& cameraStorage = m_Cameras[m_CurrentCamera];

        //

        uint32_t lastVertexBlock = std::numeric_limits<uint32_t>::max(),
                 lastIndexBlock  = std::numeric_limits<uint32_t>::max();

        nri::Buffer* lastVertexBuffer = nullptr;
        nri::Buffer* lastIndexBuffer  = nullptr;

        //

        Rhi::IndexType lastIndexType = Rhi::IndexType::MAX_NUM;

        //

        Shading::Material* lastMaterial = nullptr;

        //

        auto tryBatchBuffer =
            [](const RenderInstance&                             renderInstance,
               const Ecs::Component::BaseRenderable::BufferView& bufferView,
               Rhi::Util::BlockBasedBuffer<true>&                dynamicBuffer,
               uint32_t&                                         bufferOffset,
               nri::Buffer*&                                     nriBuffer,
               uint32_t&                                         lastBlock) -> bool
        {
            bool needsNewRow = false;
            // If the vertex or index buffer is unique, create a new row
            // (1)
            if (bufferView.HasUniqueBuffer())
            {
                needsNewRow = true;

                bufferOffset = bufferView.Offset();
                nriBuffer    = bufferView.NriBuffer;
            }
            else
            {
                auto handle = dynamicBuffer.Rent(bufferView.CpuData(), bufferView.Size());
                AME_LOG_ASSERT(Log::Gfx(), handle.Offset <= std::numeric_limits<uint32_t>::max(), "Buffer offset is too large");

                bufferOffset = static_cast<uint32_t>(handle.Offset);
                nriBuffer    = dynamicBuffer.GetBuffer(handle.BlockSlot).Unwrap();

                // if the buffer is different, create a new row
                // (2)
                if (handle.BlockSlot != lastBlock)
                {
                    needsNewRow = true;
                    lastBlock   = handle.BlockSlot;
                }
            }
            return needsNewRow;
        };

        //

        // for each entity in m_StagedEntities
        //      mutate 'Instance' member to allocate vertex/index buffer if needed
        //      if one these are true, we will need to create new row:
        //          - (1) the vertex/index buffer is owning (unique and not dynamic)
        //          - (2) the previous vertex/index buffer is different than the current one
        //          - (3) the previous index buffer type is different than the current one
        //          - (4) the previous material's pipeline state is different than the current one
        for (auto stagedEntityIter = m_StagedEntities.begin(); stagedEntityIter != m_StagedEntities.end(); stagedEntityIter++)
        {
            auto& renderInstance = stagedEntityIter->Instance.get();
            auto& renderable     = stagedEntityIter->Renderable.get();

            auto& vertex = renderable.Vertex;
            auto& index  = renderable.Index;

            bool needsNewRow = false;

            //

            // (1) + (2)
            needsNewRow |= tryBatchBuffer(
                renderInstance,
                vertex,
                cameraStorage.DynamicVertices,
                renderInstance.VertexOffset,
                lastVertexBuffer,
                lastVertexBlock);

            renderInstance.VertexSize = renderable.Vertex.Size();

            // (1) + (2)
            needsNewRow |= tryBatchBuffer(
                renderInstance,
                index,
                cameraStorage.DynamicIndices,
                renderInstance.IndexOffset,
                lastIndexBuffer,
                lastIndexBlock);

            renderInstance.IndexCount = index.Count;

            //

            // (3)
            auto indexType = renderable.GetIndexType();
            if (lastIndexType != Rhi::IndexType::MAX_NUM &&
                indexType != lastIndexType)
            {
                needsNewRow = true;
            }
            lastIndexType = indexType;

            //

            // (4)
            if (lastMaterial &&
                lastMaterial->GetPipelineHash() != renderable.Material->GetPipelineHash())
            {
                needsNewRow = true;
            }
            lastMaterial = renderable.Material.get();

            //

            cameraStorage.AllInstances.Rent(renderInstance);

            //

            if (needsNewRow)
            {
                m_StagedGroups.emplace_back(std::span{ stagedEntityIter, stagedEntityIter + 1 }, lastVertexBuffer, lastIndexBuffer);
            }
            else
            {
                auto& lastGroup    = m_StagedGroups.back();
                auto  iter         = lastGroup.Entities;
                iter               = std::span{ iter.begin(), iter.end() + 1 };
                lastGroup.Entities = iter;
            }
        }

        std::for_each(
            std::execution::par_unseq,
            m_StagedGroups.begin(),
            m_StagedGroups.end(),
            [](auto& group)
            {
                group.CalculateRMS();
            });
        std::sort(m_StagedGroups.begin(), m_StagedGroups.end());
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