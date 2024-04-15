#pragma once

#include <Rhi/Resource/CommandList.hpp>
#include <Rhi/Nri/Nri.hpp>
#include <Rhi/Device/DescriptorAllocator.hpp>

namespace Ame::Rhi
{
    class CommandListImpl : public NonCopyable,
                            public NonMovable
    {
    public:
        /// <summary>
        /// Initialize command list.
        /// </summary>
        void Initialize(
            DeviceImpl&                     RhiDevice,
            const DescriptorAllocationDesc& DescriptorPoolDesc,
            const char*                     AllocatorName,
            const char*                     ListName);

        /// <summary>
        /// Shutdown command list.
        /// </summary>
        void Shutdown();

        /// <summary>
        /// Reset command list.
        /// </summary>
        void Reset();

        /// <summary>
        /// End command list.
        /// </summary>
        void End();

    public:
        /// <summary>
        /// Get nri command list.
        /// </summary>
        [[nodiscard]] nri::CommandBuffer& GetCommandBuffer() noexcept;

        /// <summary>
        /// Submit command list to the queue.
        /// </summary>
        void Submit();

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
        /// Set descriptor sets with dynamic offsets (optional).
        /// </summary>
        void SetDescriptorSet(
            uint32_t             LayoutSlot,
            const DescriptorSet& DescriptorSets,
            uint32_t*            DynamicBufferOffset);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<SamplePosition> Positions,
            Sample_t                  SampleCount);

    public:
        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
            uint32_t LayoutSlot,
            uint32_t InstanceCount,
            uint32_t VariableCount);

    public:
        /// <summary>
        /// Set render targets and depth-stencil buffer.
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
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffers(
            std::span<const VertexBufferView> VertexBuffers,
            uint32_t                          BaseSlot);

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
        nri::CommandAllocator* m_CommandAllocator = nullptr;
        nri::CommandBuffer*    m_CommandBuffer    = nullptr;
        DeviceImpl*            m_RhiDevice        = nullptr;
        Ptr<PipelineLayout>    m_PipelineLayout;
        DescriptorAllocator    m_DescriptorAllocator;
    };
} // namespace Ame::Rhi