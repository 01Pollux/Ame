#pragma once

#include <Gfx/RG/Resources/AABBBuffer.hpp>
#include <Gfx/RG/Resources/FrameResource.hpp>
#include <Gfx/RG/Resources/InstanceBuffer.hpp>
#include <Gfx/RG/Resources/TransformBuffer.hpp>
#include <Gfx/RG/Resources/VertexBuffer.hpp>
#include <Gfx/RG/Resources/IndexBuffer.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources
    {
    public:
        CoreResources(
            Rhi::Device& Device);

    public:
        /// <summary>
        /// Get frame resource
        /// </summary>
        [[nodiscard]] const Rhi::Buffer& GetFrameResource() const;

        /// <summary>
        /// Get frame resource data
        /// </summary>
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

        /// <summary>
        /// Update frame resource for the current frame
        /// </summary>
        void UpdateFrameResource(
            float                        EngineTime,
            float                        GameTime,
            float                        DeltaTime,
            const Math::TransformMatrix& Transform,
            const Math::Matrix4x4&       Projection,
            const Math::Vector2&         Viewport);

    private:
        /// <summary>
        /// Allocate the frame resource
        /// </summary>
        void AllocateFrameResource();

    private:
        Ref<Rhi::Device> m_Device;

        Rhi::Buffer      m_FrameResourceBuffer;
        FrameResourceCPU m_FrameResource;
    };
} // namespace Ame::Gfx::RG