#pragma once

#include <Graphics/EntityCompositor/Core.hpp>

#include <Graphics/EntityCompositor/EntityGpuStorage/TransformTable.hpp>
#include <Graphics/EntityCompositor/EntityGpuStorage/DrawInstanceTable.hpp>
#include <Graphics/EntityCompositor/EntityGpuStorage/LightTable.hpp>

namespace Ame::Gfx
{
    class EntityStorage
    {
        static constexpr uint32_t c_DrawCommandsChunkSize = 1024;

    private:
        friend class EntityCollector;

    public:
        EntityStorage(Rhi::IRhiDevice* rhiDevice, Ecs::World* world);

    private:
        void UploadToRenderGraph(Rg::Graph& cameraGraph, const CameraFrameDataUpdateDesc& frameData);

    private:
        void UpdateInstances();

        void CreateFrameDataBuffer();
        void UpdateFrameData(const CameraFrameDataUpdateDesc& frameData);

        void CreateLightIdBuffer();
        // First instance of light id is light count
        void UpdateLightInstances(std::span<const uint32_t> lightIds);

    private:
        void UploadAllResource(Rg::Graph& cameraGraph);

    public:
        [[nodiscard]] Rhi::IRhiDevice* GetRenderDevice();
        [[nodiscard]] Ecs::World*      GetWorld();

    private:
        Rhi::IRhiDevice* m_RhiDevice;
        Ecs::World*      m_World;

        Ptr<Dg::IBuffer> m_FrameDataBuffer; // FrameData
        Ptr<Dg::IBuffer> m_LightIdBuffer;   // uint32_t[]

        // Ptr<Dg::IBuffer>             m_AABBBuffer;              // Geometry::AABB[]
        // Ptr<Dg::IBuffer>             m_SphereBuffer;            // Geometry::Sphere[]
        EntityTransformGpuStorage    m_TransformStorage;    // Transform[]
        EntityDrawInstanceGpuStorage m_DrawInstanceStorage; // EntityDrawInstance[]
        EntityLightGpuStorage        m_LightStorage;        // Light[]
    };
} // namespace Ame::Gfx