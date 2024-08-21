#include <Graphics/EntityCompositor/Storage.hpp>

#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Graphics/RenderGraph/Common/Constants.hpp>

namespace Ame::Gfx
{
    EntityStorage::EntityStorage(
        Rhi::IRhiDevice* rhiDevice,
        Ecs::World*      world) :
        m_World(world),
        m_RhiDevice(rhiDevice),
        m_TransformStorage(*world),
        m_DrawInstanceStorage(*world),
        m_LightStorage(*world)
    {
        CreateFrameDataBuffer();
        CreateLightIdBuffer();
    }

    //

    void EntityStorage::UploadToRenderGraph(
        Rg::Graph&                       cameraGraph,
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

        m_TransformStorage.Upload(renderDevice, immediateContext);
        m_DrawInstanceStorage.Upload(renderDevice, immediateContext);
        m_LightStorage.Upload(renderDevice, immediateContext);
    }

    void EntityStorage::CreateFrameDataBuffer()
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

    void EntityStorage::UpdateFrameData(
        const CameraFrameDataUpdateDesc& updateDesc)
    {
        auto renderContext = m_RhiDevice->GetImmediateContext();

        CameraFrameData frameData{
            .World = updateDesc.World,

            .View           = updateDesc.View,
            .Projection     = updateDesc.Projection,
            .ViewProjection = updateDesc.View * updateDesc.Projection,

            .ViewInverse           = updateDesc.View.GetInverse(),
            .ProjectionInverse     = updateDesc.Projection.GetInverse(),
            .ViewProjectionInverse = frameData.ViewProjection.GetInverse(),

            .Viewport = updateDesc.Viewport,

            .EngineTime = updateDesc.EngineTime,
            .GameTime   = updateDesc.GameTime,
            .DeltaTime  = updateDesc.DeltaTime,

            .NearPlane = updateDesc.NearPlane,
            .FarPlane  = updateDesc.FarPlane,

            .CameraMask = updateDesc.CameraMask
        };
        renderContext->UpdateBuffer(m_FrameDataBuffer, 0, sizeof(frameData), &frameData, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void EntityStorage::CreateLightIdBuffer()
    {
        Dg::BufferDesc bufferDesc{
            "LightIdBuffer",
            sizeof(uint32_t) * c_MaxLightConstants,
            Dg::BIND_SHADER_RESOURCE,
            Dg::USAGE_DEFAULT,
            Dg::CPU_ACCESS_NONE,
            Dg::BUFFER_MODE_STRUCTURED,
            sizeof(uint32_t)
        };
        auto renderDevice = m_RhiDevice->GetRenderDevice();
        renderDevice->CreateBuffer(bufferDesc, nullptr, &m_LightIdBuffer);
    }

    void EntityStorage::UpdateLightInstances(
        std::span<const uint32_t> lightIds)
    {
        size_t maxCount      = std::min(lightIds[0] + 1, c_MaxLightConstants);
        auto   renderContext = m_RhiDevice->GetImmediateContext();
        renderContext->UpdateBuffer(m_LightIdBuffer, 0, sizeof(uint32_t) * maxCount, lightIds.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    //

    void EntityStorage::UploadAllResource(
        Rg::Graph& cameraGraph)
    {
        auto& resourceStorage = cameraGraph.GetResourceStorage();

        resourceStorage.ImportBuffer(c_RGFrameData, m_FrameDataBuffer);
        resourceStorage.ImportBuffer(c_RGTransformTable, m_TransformStorage.GetBuffer());
        resourceStorage.ImportBuffer(c_RGRenderInstanceTable, m_DrawInstanceStorage.GetBuffer());
        resourceStorage.ImportBuffer(c_RGLightInstanceTable, m_LightStorage.GetBuffer());
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