#pragma once

#include <Rhi/Resource/CommandList.hpp>
#include <Rhi/Nri/Nri.hpp>

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
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue,
            const char*         AllocatorName,
            const char*         ListName);

        /// <summary>
        /// Shutdown command list.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Reset command list.
        /// </summary>
        void Reset(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// End command list.
        /// </summary>
        void End(
            nri::CoreInterface& NriCore);

    public:
        /// <summary>
        /// Get nri command list.
        /// </summary>
        [[nodiscard]] nri::CommandBuffer& GetCommandBuffer() noexcept;

        /// <summary>
        /// Submit command list to the queue.
        /// </summary>
        void Submit(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

    public:
        /// <summary>
        /// Set pipeline layout.
        /// </summary>
        void SetPipelineLayout(
            DeviceImpl&     RhiDevice,
            PipelineLayout& Layout);

        /// <summary>
        /// Set pipeline state.
        /// </summary>
        void SetPipelineState(
            DeviceImpl&    RhiDevice,
            PipelineState& Pipeline);

    public:
        /// <summary>
        /// Set constants.
        /// </summary>
        void SetConstants(
            DeviceImpl& RhiDevice,
            uint32_t    ConstantIndex,
            const void* Data,
            size_t      Size);

        /// <summary>
        /// Set descriptor sets with dynamic offsets (optional).
        /// </summary>
        void SetDescriptorSet(
            DeviceImpl&          RhiDevice,
            uint32_t             LayoutSlot,
            const DescriptorSet& DescriptorSets,
            uint32_t*            DynamicBufferOffset);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            DeviceImpl&               RhiDevice,
            std::span<SamplePosition> Positions,
            Sample_t                  SampleCount);

    public:
        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet*> AllocateSets(
            DeviceImpl& RhiDevice,
            uint32_t    Count);

    public:
        /// <summary>
        /// Set render targets and depth-stencil buffer.
        /// </summary>
        void BeginRendering(
            DeviceImpl&                  RhiDevice,
            std::span<Rhi::ResourceView> RenderTargets,
            Rhi::ResourceView            DepthStencil = Rhi::ResourceView{});

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            DeviceImpl&            RhiDevice,
            std::span<ClearDesc>   Clears,
            std::span<ClearRegion> Regions);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewports(
            DeviceImpl&         RhiDevice,
            std::span<Viewport> Viewports);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            DeviceImpl&            RhiDevice,
            std::span<ScissorRect> ScissorRects);

        /// <summary>
        /// Set stencil reference value.
        /// </summary>
        void SetStencilReference(
            DeviceImpl& RhiDevice,
            uint8_t     StencilReference);

        /// <summary>
        /// Set depth bounds.
        /// </summary>
        void SetDepthBounds(
            DeviceImpl& RhiDevice,
            float       MinDepthBounds,
            float       MaxDepthBounds);

        /// <summary>
        /// Set blend constants.
        /// </summary>
        void SetBlendConstants(
            DeviceImpl&         RhiDevice,
            const Math::Color4& BlendConstants);

        /// <summary>
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffers(
            DeviceImpl&                       RhiDeviceImpl,
            class Device&                     RhiDevice,
            std::span<const VertexBufferView> VertexBuffers,
            uint32_t                          BaseSlot);

        /// <summary>
        /// Set index buffer.
        /// </summary>
        void SetIndexBuffer(
            DeviceImpl&            RhiDeviceImpl,
            class Device&          RhiDevice,
            const IndexBufferView& IndexBuffer);

        /// <summary>
        /// Perform draw call.
        /// </summary>
        void Draw(
            DeviceImpl&     RhiDevice,
            const DrawDesc& Desc);

        /// <summary>
        /// Perform draw indexed call.
        /// </summary>
        void Draw(
            DeviceImpl&            RhiDevice,
            const DrawIndexedDesc& Desc);

        /// <summary>
        /// End rendering.
        /// </summary>
        void EndRendering(
            DeviceImpl& RhiDevice);

    private:
        nri::CommandAllocator* m_CommandAllocator = nullptr;
        nri::CommandBuffer*    m_CommandBuffer    = nullptr;
    };
} // namespace Ame::Rhi