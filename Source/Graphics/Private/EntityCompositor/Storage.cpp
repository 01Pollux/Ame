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
        m_TransformStorage(*world),
        m_DrawInstanceStorage(*world)
    {
        Dg::BufferDesc bufferDesc{
            "FrameDataBuffer",
            sizeof(CameraFrameData),
            Dg::BIND_UNIFORM_BUFFER,
            Dg::USAGE_DEFAULT
        };

        auto renderDevice = m_RhiDevice->GetRenderDevice();
        renderDevice->CreateBuffer(bufferDesc, nullptr, &m_FrameDataBuffer);
    }

    //

    void EntityStorage::UploadToRenderGraph(
        RG::Graph&                       cameraGraph,
        const CameraFrameDataUpdateDesc& frameData)
    {
        UpdateInstances();
        UpdateFrameData(frameData);
        UploadAllResource(cameraGraph);
    }

    //

    void EntityStorage::UpdateInstances()
    {
        auto renderDevice     = m_RhiDevice->GetRenderDevice();
        auto immediateContext = m_RhiDevice->GetImmediateContext();

        m_TransformStorage.Upload(*m_World, renderDevice, immediateContext);
        m_DrawInstanceStorage.Upload(*m_World, renderDevice, immediateContext);
    }

    void EntityStorage::UpdateFrameData(
        const CameraFrameDataUpdateDesc& updateDesc)
    {
        auto renderContext = m_RhiDevice->GetImmediateContext();

        CameraFrameData frameData{
            .World = updateDesc.WorldTransposed,

            .View           = updateDesc.ViewTransposed,
            .Projection     = updateDesc.ProjectionTransposed,
            .ViewProjection = updateDesc.ViewTransposed * updateDesc.ProjectionTransposed,

            .ViewInverse           = updateDesc.ViewTransposed.GetInverse(),
            .ProjectionInverse     = updateDesc.ProjectionTransposed.GetInverse(),
            .ViewProjectionInverse = frameData.ViewProjection.GetInverse(),

            .Viewport = updateDesc.Viewport,

            .EngineTime = updateDesc.EngineTime,
            .GameTime   = updateDesc.GameTime,
            .DeltaTime  = updateDesc.DeltaTime
        };
        renderContext->UpdateBuffer(m_FrameDataBuffer, 0, sizeof(frameData), &frameData, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    //

    void EntityStorage::UploadAllResource(
        RG::Graph& cameraGraph)
    {
        auto& resourceStorage = cameraGraph.GetResourceStorage();

        resourceStorage.ImportBuffer(c_RGFrameData, m_FrameDataBuffer);
        resourceStorage.ImportBuffer(c_RGTransformTable, m_TransformStorage.GetBuffer());
        resourceStorage.ImportBuffer(c_RGRenderInstanceTable, m_DrawInstanceStorage.GetBuffer());
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