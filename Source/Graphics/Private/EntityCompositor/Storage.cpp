#include <Graphics/EntityCompositor/Storage.hpp>

#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Rhi/Utils/IndirectCommandStructs.hpp>

namespace Ame::Gfx
{
    EntityStorage::EntityStorage(
        Rhi::IRhiDevice* rhiDevice,
        Ecs::World*      world) :
        m_World(world),
        m_RhiDevice(rhiDevice),
        m_DrawInstanceStorage(*world)
    {
        Dg::BufferDesc bufferDesc{
            "FrameDataBuffer",
            sizeof(CameraFrameData),
            Dg::BIND_UNIFORM_BUFFER,
            Dg::USAGE_DYNAMIC,
            Dg::CPU_ACCESS_WRITE
        };

        auto renderDevice = m_RhiDevice->GetRenderDevice();
        renderDevice->CreateBuffer(bufferDesc, nullptr, &m_FrameDataBuffer);
    }

    void EntityStorage::WriteInstanceIndices(
        const EntityDrawCommandsCategory& sortedInstances)
    {
        auto flatIndices =
            sortedInstances |
            std::views::join |
            std::views::transform([](auto& instance)
                                  { return instance.InstanceId; }) |
            std::ranges::to<std::vector>();

        UpdateInstances();
        UpdateInstanceIndices(flatIndices);
    }

    //

    void EntityStorage::UploadToRenderGraph(
        RG::Graph&                       cameraGraph,
        const CameraFrameDataUpdateDesc& frameData)
    {
        UpdateFrameData(frameData);
        UploadAllResource(cameraGraph);
    }

    //

    void EntityStorage::UpdateInstances()
    {
        m_DrawInstanceStorage.Upload(
            *m_World,
            m_RhiDevice->GetRenderDevice(),
            m_RhiDevice->GetImmediateContext());
    }

    void EntityStorage::UpdateInstanceIndices(
        std::span<const uint32_t> indices)
    {
        size_t requiredSize = m_DrawInstanceStorage.GetMaxCount() * sizeof(uint32_t);
        if (!m_DrawInstanceIndexBuffer || m_DrawInstanceIndexBuffer->GetDesc().Size < requiredSize)
        {
            auto renderDevice = m_RhiDevice->GetRenderDevice();

            Dg::BufferDesc bufferDesc{
                "DrawInstanceIndexBuffer",
                requiredSize,
                Dg::BIND_SHADER_RESOURCE,
                Dg::USAGE_DYNAMIC,
                Dg::CPU_ACCESS_WRITE,
                Dg::BUFFER_MODE_STRUCTURED,
                sizeof(uint32_t)
            };

            m_DrawInstanceIndexBuffer.Release();
            renderDevice->CreateBuffer(bufferDesc, nullptr, &m_DrawInstanceIndexBuffer);
        }

        if (!indices.empty())
        {
            auto immediateContext = m_RhiDevice->GetImmediateContext();
            // immediateContext->UpdateBuffer(m_DrawInstanceIndexBuffer, 0, indices.size_bytes(), indices.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
            Dg::MapHelper<uint32_t> mappedData(immediateContext, m_DrawInstanceIndexBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
            std::copy(indices.begin(), indices.end(), static_cast<uint32_t*>(mappedData));
        }
    }

    void EntityStorage::UpdateFrameData(
        const CameraFrameDataUpdateDesc& updateDesc)
    {
        auto renderContext = m_RhiDevice->GetImmediateContext();

        CameraFrameData frameData{
            .World = updateDesc.WorldTransposed,

            .View           = updateDesc.ViewTransposed,
            .Projection     = updateDesc.ProjectionTransposed,
            .ViewProjection = updateDesc.ProjectionTransposed * updateDesc.ViewTransposed,

            .ViewInverse           = updateDesc.ViewTransposed.GetInverse(),
            .ProjectionInverse     = updateDesc.ProjectionTransposed.GetInverse(),
            .ViewProjectionInverse = frameData.ViewProjection.GetInverse(),

            .Viewport = updateDesc.Viewport,

            .EngineTime = updateDesc.EngineTime,
            .GameTime   = updateDesc.GameTime,
            .DeltaTime  = updateDesc.DeltaTime
        };
        // renderContext->UpdateBuffer(m_FrameDataBuffer, 0, sizeof(frameData), &frameData, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Dg::MapHelper<CameraFrameData> mappedData(renderContext, m_FrameDataBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
        *mappedData = frameData;
    }

    //

    void EntityStorage::UploadAllResource(
        RG::Graph& cameraGraph)
    {
        auto& resourceStorage = cameraGraph.GetResourceStorage();

        resourceStorage.ImportBuffer(c_RGFrameData, m_FrameDataBuffer);

        resourceStorage.ImportBuffer(c_RGRenderInstances, m_DrawInstanceStorage.GetBuffer());
        resourceStorage.ImportBuffer(c_RGSortedRenderInstances, m_DrawInstanceIndexBuffer);
    }

    //

    Rhi::IRhiDevice* EntityStorage::GetRenderDevice()
    {
        return m_RhiDevice;
    }

    Ecs::World* EntityStorage::GetWorld()
    {
        return m_World;
    }
} // namespace Ame::Gfx