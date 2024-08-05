#include <Graphics/EntityCompositor/Storage.hpp>

#include <Graphics/EntityCompositor/RenderGraphResources.hpp>
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

        UpdateInstanceIndices(flatIndices);
        UpdateDrawCommands(flatIndices);
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
                Dg::CPU_ACCESS_FLAGS::CPU_ACCESS_WRITE,
                Dg::BUFFER_MODE_STRUCTURED,
                sizeof(uint32_t)
            };
            Dg::BufferData bufferData(indices.data(), indices.size_bytes());

            m_DrawInstanceIndexBuffer.Release();
            renderDevice->CreateBuffer(bufferDesc, &bufferData, &m_DrawInstanceIndexBuffer);
        }
        else
        {
            auto      immediateContext = m_RhiDevice->GetImmediateContext();
            Dg::PVoid mappedData       = nullptr;

            immediateContext->MapBuffer(m_DrawInstanceIndexBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_NONE, mappedData);
            std::memcpy(mappedData, indices.data(), indices.size_bytes());
            immediateContext->UnmapBuffer(m_DrawInstanceIndexBuffer, Dg::MAP_WRITE);
        }
    }

    void EntityStorage::UpdateDrawCommands(
        std::span<const uint32_t> indices)
    {
        auto indicesCount = Math::AlignUp(static_cast<uint32_t>(indices.size()), c_DrawCommandsChunkSize);
        if (!m_DrawCounterBuffer || m_DrawCounterBuffer->GetDesc().Size < (indicesCount * sizeof(uint32_t)))
        {
            Dg::BufferDesc bufferDesc{
#ifndef AME_DIST
                "DrawCounterBuffer",
#else
                nullptr,
#endif
                indicesCount * sizeof(uint32_t),
                Dg::BIND_INDIRECT_DRAW_ARGS | Dg::BIND_UNORDERED_ACCESS
            };

            auto renderDevice = m_RhiDevice->GetRenderDevice();
            renderDevice->CreateBuffer(bufferDesc, nullptr, &m_DrawCounterBuffer);

#ifndef AME_DIST
            bufferDesc.Name = "DrawCommandBuffer";
#endif
            bufferDesc.Size = indicesCount * sizeof(Rhi::DrawIndexedIndirectCommand);
            renderDevice->CreateBuffer(bufferDesc, nullptr, &m_DrawCommandBuffer);
        }
    }

    void EntityStorage::UpdateFrameData(
        const CameraFrameDataUpdateDesc& frameData)
    {
        auto renderContext = m_RhiDevice->GetImmediateContext();

        Dg::MapHelper<CameraFrameData> frameDataMap(renderContext, m_FrameDataBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_NONE);
        frameDataMap->World      = frameData.WorldTransposed;
        frameDataMap->View       = frameData.ViewTransposed;
        frameDataMap->Projection = frameData.ProjectionTransposed;

        frameDataMap->Viewport = frameData.Viewport;

        frameDataMap->EngineTime = frameData.EngineTime;
        frameDataMap->GameTime   = frameData.GameTime;
        frameDataMap->DeltaTime  = frameData.DeltaTime;

        auto viewProjection                 = frameData.ProjectionTransposed * frameData.ViewTransposed;
        frameDataMap->ViewProjection        = viewProjection;
        frameDataMap->ViewInverse           = frameData.ViewTransposed.GetInverse();
        frameDataMap->ProjectionInverse     = frameData.ProjectionTransposed.GetInverse();
        frameDataMap->ViewProjectionInverse = viewProjection.GetInverse();
    }

    //

    void EntityStorage::UploadAllResource(
        RG::Graph& cameraGraph)
    {
        auto& resourceStorage = cameraGraph.GetResourceStorage();

        resourceStorage.ImportBuffer(RG_FrameDataResource, m_FrameDataBuffer);
        resourceStorage.ImportBuffer(RG_DrawCommandResource, m_DrawCommandBuffer);
        resourceStorage.ImportBuffer(RG_DrawCounterResource, m_DrawCounterBuffer);
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