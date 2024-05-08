#pragma once

#include <Rhi/CommandList/CommandList.hpp>
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
        /// Begin tag marker.
        /// </summary>
        void BeginMarker(
            const char* Name);

        /// <summary>
        /// End last tag marker.
        /// </summary>
        void EndMarker();

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
            std::span<const Rhi::ResourceView*> RenderTargets,
            const Rhi::ResourceView*            DepthStencil);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<ClearDesc>   Clears,
            std::span<ClearRegion> Regions);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<ClearDesc> Clears);

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

    public:
        /// <summary>
        /// Perform a copy from buffer to buffer
        /// Copy the rest of buffer if size is 0
        /// </summary>
        void CopyBuffer(
            const BufferCopyDesc& Src,
            const BufferCopyDesc& Dst,
            size_t                Size = 0);

        /// <summary>
        /// Perform a copy from texture to texture
        /// </summary>
        void CopyTexture(
            const TextureCopyDesc& Src,
            const TextureCopyDesc& Dst);

        /// <summary>
        /// Perform a copy from buffer to texture
        /// </summary>
        void UploadTexture(
            const TransferCopyDesc& Desc);

        /// <summary>
        /// Perform a copy from texture to buffer
        /// </summary>
        void ReadbackTexture(
            const TransferCopyDesc& Desc);

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Buffer&      RhiBuffer,
            const AccessStage& State,
            bool               Append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Texture&            RhiTexture,
            const AccessLayoutStage&  State,
            const TextureSubresource& Subresource = AllSubresources,
            bool                      Append      = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const GlobalBarrierDesc& BarrierDesc);

        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers();

    public:
        /// <summary>
        /// Clear storage buffer
        /// </summary>
        void ClearBuffer(
            const ClearBufferDesc& Desc);

        /// <summary>
        /// Clear storage texture
        /// </summary>
        void ClearTexture(
            const ClearTextureDesc& Desc);

    private:
        nri::CommandAllocator* m_CommandAllocator = nullptr;
        nri::CommandBuffer*    m_CommandBuffer    = nullptr;
        DeviceImpl*            m_RhiDevice        = nullptr;
        Ptr<PipelineLayout>    m_PipelineLayout;
        DescriptorAllocator    m_DescriptorAllocator;
    };
} // namespace Ame::Rhi