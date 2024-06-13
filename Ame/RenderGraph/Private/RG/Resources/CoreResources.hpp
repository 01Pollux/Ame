#pragma once

#include <RG/Resources/FrameResource.hpp>

namespace Ame::RG
{
    class CoreResources
    {
    public:
        CoreResources(
            Rhi::Device& rhiDevice);

    public:
        [[nodiscard]] const Rhi::Buffer&      GetFrameResource() const;
        [[nodiscard]] Rhi::BufferViewDesc     GetFrameResourceViewDesc() const;
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

    public:
        /// <summary>
        /// Update frame resource for the current frame
        /// </summary>
        void UpdateFrameResource(
            float                        engineTime,
            float                        gameTime,
            float                        deltaTime,
            const Ecs::Entity&           cameraEntity,
            const Math::TransformMatrix& transform,
            const Math::Matrix4x4&       projection,
            const Math::Vector2&         viewport);

    private:
        /// <summary>
        /// Allocate the frame resource
        /// </summary>
        [[nodiscard]] static Rhi::Buffer AllocateFrameResource(
            Rhi::Device& rhiDevice);

    private:
        Ref<Rhi::Device> m_Device;

        Rhi::Buffer      m_FrameResourceBuffer;
        FrameResourceCPU m_FrameResource;
    };
} // namespace Ame::RG