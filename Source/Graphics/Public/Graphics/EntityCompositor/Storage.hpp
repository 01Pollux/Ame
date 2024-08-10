#pragma once

#include <Graphics/EntityCompositor/Core.hpp>
#include <Graphics/EntityCompositor/EntityGpuStorage/DrawInstance.hpp>

namespace Ame::Gfx
{
    class EntityStorage
    {
        static constexpr uint32_t c_DrawCommandsChunkSize = 1024;

    private:
        friend class EntityCollector;

    public:
        EntityStorage(
            Rhi::IRhiDevice* rhiDevice,
            Ecs::World*      world);

    private:
        void WriteInstanceIndices(
            const EntityDrawCommandsCategory& sortedInstances);

        void UploadToRenderGraph(
            RG::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);

    private:
        void UpdateInstances();

        void UpdateInstanceIndices(
            std::span<const uint32_t> indices);

        void UpdateFrameData(
            const CameraFrameDataUpdateDesc& frameData);

    private:
        void UploadAllResource(
            RG::Graph& cameraGraph);

    public:
        [[nodiscard]] Rhi::IRhiDevice* GetRenderDevice();
        [[nodiscard]] Ecs::World*      GetWorld();

    private:
        Rhi::IRhiDevice* m_RhiDevice;
        Ecs::World*      m_World;

        Ptr<Dg::IBuffer> m_FrameDataBuffer; // FrameData

        // Ptr<Dg::IBuffer>             m_TransformBuffer;         // matrix[]
        // Ptr<Dg::IBuffer>             m_AABBBuffer;              // Geometry::AABB[]
        // Ptr<Dg::IBuffer>             m_SphereBuffer;            // Geometry::Sphere[]
        EntityDrawInstanceGpuStorage m_DrawInstanceStorage;     // EntityDrawInstance[]
        Ptr<Dg::IBuffer>             m_DrawInstanceIndexBuffer; // uint32_t[]
    };
} // namespace Ame::Gfx