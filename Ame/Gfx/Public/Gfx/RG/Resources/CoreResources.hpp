#pragma once

#include <Gfx/RG/Resources/FrameResource.hpp>
#include <Gfx/RG/Resources/AABBBuffer.hpp>
#include <Gfx/RG/Resources/TransformBuffer.hpp>
#include <Gfx/RG/Resources/VertexBuffer.hpp>
#include <Gfx/RG/Resources/IndexBuffer.hpp>
#include <Gfx/RG/Resources/InstanceBuffer.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources
    {
    public:
        CoreResources(
            Rhi::Device& Device);

    public:
        [[nodiscard]] const Rhi::Buffer&      GetFrameResource() const;
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

    public:
        [[nodiscard]] const AABBBuffer&      GetAABBBuffer() const;
        [[nodiscard]] const TransformBuffer& GetTransformBuffer() const;
        [[nodiscard]] const VertexBuffer&    GetVertexBuffer() const;
        [[nodiscard]] const IndexBuffer&     GetIndexBuffer() const;
        [[nodiscard]] const InstanceBuffer&  GetInstanceBuffer() const;

    public:
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
        [[nodiscard]] static Rhi::Buffer AllocateFrameResource(
            Rhi::Device& Device);

    private:
        Ref<Rhi::Device> m_Device;

        Rhi::Buffer      m_FrameResourceBuffer;
        FrameResourceCPU m_FrameResource;

        AABBBuffer      m_AABBBuffer;
        TransformBuffer m_TransformBuffer;
        VertexBuffer    m_VertexBuffer;
        IndexBuffer     m_IndexBuffer;
        InstanceBuffer  m_InstanceBuffer;
    };
} // namespace Ame::Gfx::RG