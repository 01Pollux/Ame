#pragma once

#include <Graphics/EntityCompositor/Core.hpp>

namespace Ame::Gfx
{
    class EntityStorage
    {
    private:
        friend class EntityCollector;
        friend class EntityDrawer;

    public:
        EntityStorage(Rhi::IRhiDevice* rhiDevice) :
            m_RhiDevice(rhiDevice)
        {
            InstallHooks();
        }

    private:
        void WriteInstances(
            std::span<const EntityDrawInstance> instances);

        void WriteInstanceIndices(
            const SortedEntityDrawCommandsCategory& instanceIndices);

    private:
        void ReserveIndirectBuffers(
            uint32_t maxDrawCount);

    private:
        void UploadToRenderGraph(
            RG::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);

    private:
        void InstallHooks();

    private:
        Rhi::IRhiDevice* m_RhiDevice;
        Ptr<Dg::IBuffer> m_FrameDataBuffer; // FrameData

        Ptr<Dg::IBuffer> m_TransformBuffer;         // matrix[]
        Ptr<Dg::IBuffer> m_AABBBuffer;              // Geometry::AABB[]
        Ptr<Dg::IBuffer> m_SphereBuffer;            // Geometry::Sphere[]
        Ptr<Dg::IBuffer> m_DrawInstanceBuffer;      // EntityDrawInstance[]
        Ptr<Dg::IBuffer> m_DrawInstanceIndexBuffer; // uint32_t[]

        Ptr<Dg::IBuffer> m_DrawCounterBuffer; // uint32_t[]
        Ptr<Dg::IBuffer> m_DrawCommandBuffer; // DrawIndexedIndirectCommand[]
    };
} // namespace Ame::Gfx