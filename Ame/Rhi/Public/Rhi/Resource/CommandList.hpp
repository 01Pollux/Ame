#pragma once

#include <Rhi/Descs/CommandList.hpp>

#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/Set.hpp>

#include <Rhi/Stream/Buffer.hpp>

namespace Ame::Rhi
{
    struct VertexBufferView;
    struct IndexBufferView;

    /// <summary>
    /// Command list is a sequence of commands that can be executed by the GPU.
    /// It is used to record rendering commands, dispatch compute shaders, and copy resources.
    ///
    /// They are not synchronized, and creating multiple command lists with the same device WILL reference the same device object.
    /// </summary>
    class CommandList
    {
    public:
        CommandList(
            Device& RhiDevice);

    public:
        /// <summary>
        /// Set pipeline layout.
        /// </summary>
        void SetPipelineLayout(
            const Ptr<PipelineLayout>& Layout);

        /// <summary>
        /// Set pipeline state.
        /// </summary>
        void SetPipelineState(
            const Ptr<PipelineState>& Pipeline);

    public:
        /// <summary>
        /// Set constants.
        /// </summary>
        void SetConstants(
            uint32_t    ConstantIndex,
            const void* Data,
            size_t      Size);

        /// <summary>
        /// Set constants.
        /// </summary>
        template<typename Ty>
            requires std::is_standard_layout_v<Ty>
        void SetConstants(
            uint32_t  ConstantIndex,
            const Ty& Data)
        {
            SetConstants(ConstantIndex, &Data, sizeof(Ty));
        }

        /// <summary>
        /// Set descriptor sets.
        /// </summary>
        void SetDescriptorSet(
            uint32_t             LayoutSlot,
            const DescriptorSet& DescriptorSets);

        /// <summary>
        /// Set descriptor sets with dynamic offsets.
        /// </summary>
        void SetDescriptorSet(
            uint32_t             LayoutSlot,
            const DescriptorSet& DescriptorSets,
            uint32_t             DynamicBufferOffset);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<SamplePosition> Positions,
            Sample_t                  SampleCount);

    public:
        /// <summary>
        /// Allocate a buffer for upload.
        /// The buffer is CPU-GPU visible and can be used to stream data to the GPU.
        /// WindowSize is the size of intermediate buffer used for streaming.
        /// </summary>
        [[nodiscard]] Streaming::BufferOStream AllocateUpload(
            size_t Size,
            size_t WindowSize = 0);

        /// <summary>
        /// Allocate a buffer for upload.
        /// The buffer is GPU visible and can be used to stream data to the GPU.
        /// </summary>
        [[nodiscard]] Streaming::BufferOStream AllocateScratch(
            size_t Size,
            size_t WindowSize = 0);

        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
            uint32_t LayoutSlot,
            uint32_t InstanceCount = 1,
            uint32_t VariableCount = 0);

    public:
        /// <summary>
        /// Set render targets.
        /// </summary>
        void BeginRendering(
            std::span<Rhi::ResourceView> RenderTargets,
            Rhi::ResourceView            DepthStencil = Rhi::ResourceView{});

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<ClearDesc>   Clears,
            std::span<ClearRegion> Regions);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewports(
            std::span<Viewport> Viewports);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            std::span<ScissorRect> ScissorRects);

        /// <summary>
        /// Set stencil reference value.
        /// </summary>
        void SetStencilReference(
            uint8_t StencilReference);

        /// <summary>
        /// Set depth bounds.
        /// </summary>
        void SetDepthBounds(
            float MinDepthBounds,
            float MaxDepthBounds);

        /// <summary>
        /// Set blend constants.
        /// </summary>
        void SetBlendConstants(
            const Math::Color4& BlendConstants);

        /// <summary>
        /// Set vertex buffers.
        /// </summary>
        void SetVertexBuffers(
            std::span<const VertexBufferView> VertexBuffers,
            uint32_t                          BaseSlot = 0);

        /// <summary>
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffer(
            const VertexBufferView& VertexBuffer,
            uint32_t                BaseSlot = 0);

        /// <summary>
        /// Set index buffer.
        /// </summary>
        void SetIndexBuffer(
            const IndexBufferView& IndexBuffer);

        /// <summary>
        /// Perform draw call.
        /// </summary>
        void Draw(
            const DrawDesc& Desc);

        /// <summary>
        /// Perform draw indexed call.
        /// </summary>
        void Draw(
            const DrawIndexedDesc& Desc);

        /// <summary>
        /// End rendering.
        /// </summary>
        void EndRendering();

    private:
        Device&          m_Device;
        CommandListImpl& m_Impl;
    };
} // namespace Ame::Rhi