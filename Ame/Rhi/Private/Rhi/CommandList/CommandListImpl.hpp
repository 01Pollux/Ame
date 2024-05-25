#pragma once

#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Nri/Nri.hpp>

#include <Rhi/Device/DescriptorAllocator.hpp>

namespace Ame::Rhi
{
    class CommandListImpl
    {
    public:
        CommandListImpl() = default;

        CommandListImpl(
            const CommandListImpl&) = delete;
        CommandListImpl& operator=(
            const CommandListImpl&) = delete;

        CommandListImpl(
            CommandListImpl&&) = delete;
        CommandListImpl& operator=(
            CommandListImpl&&) = delete;

        ~CommandListImpl() = default;

    public:
        /// <summary>
        /// Initialize command list.
        /// </summary>
        void Initialize(
            DeviceImpl&                     rhiDevice,
            const DescriptorAllocationDesc& allocationDesc,
            const char*                     allocatorName,
            const char*                     listName);

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
            const char* name);

        /// <summary>
        /// End last tag marker.
        /// </summary>
        void EndMarker();

    public:
        /// <summary>
        /// Set pipeline layout.
        /// </summary>
        void SetPipelineLayout(
            const Ptr<PipelineLayout>& layout);

        /// <summary>
        /// Set pipeline state.
        /// </summary>
        void SetPipelineState(
            const Ptr<PipelineState>& pipeline);

    public:
        /// <summary>
        /// Set constants.
        /// </summary>
        void SetConstants(
            uint32_t    constantIndex,
            const void* data,
            size_t      size);

        /// <summary>
        /// Set descriptor sets with dynamic offsets (optional).
        /// </summary>
        void SetDescriptorSet(
            uint32_t             layoutSlot,
            const DescriptorSet& descriptorSets,
            const uint32_t*      dynamicBufferOffset);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<const SamplePosition> positions,
            Sample_t                        sampleCount);

    public:
        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] DescriptorSet AllocateSet(
            uint32_t layoutSlot,
            uint32_t variableCount);

        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
            uint32_t layoutSlot,
            uint32_t instanceCount,
            uint32_t variableCount);

    public:
        /// <summary>
        /// Set render targets and depth-stencil buffer.
        /// </summary>
        void BeginRendering(
            std::span<const Rhi::ResourceView*> renderTargets,
            const Rhi::ResourceView*            depthStencil);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<const ClearDesc>   clears,
            std::span<const ClearRegion> regions);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<const ClearDesc> clears);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewports(
            std::span<const Viewport> viewports);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            std::span<const ScissorRect> scissorRects);

        /// <summary>
        /// Set stencil reference value.
        /// </summary>
        void SetStencilReference(
            uint8_t stencilReference);

        /// <summary>
        /// Set depth bounds.
        /// </summary>
        void SetDepthBounds(
            float minDepthBounds,
            float maxDepthBounds);

        /// <summary>
        /// Set blend constants.
        /// </summary>
        void SetBlendConstants(
            const Math::Color4& blendConstants);

        /// <summary>
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffers(
            std::span<const VertexBufferView> vertexBuffers,
            uint32_t                          baseSlot);

        /// <summary>
        /// Set index buffer.
        /// </summary>
        void SetIndexBuffer(
            const IndexBufferView& indexBuffer);

        /// <summary>
        /// Perform draw call.
        /// </summary>
        void Draw(
            const DrawDesc& drawDesc);

        /// <summary>
        /// Perform draw indexed call.
        /// </summary>
        void Draw(
            const DrawIndexedDesc& drawDesc);

        /// <summary>
        /// Perform an indirect draw call.
        /// </summary>
        void DrawIndirect(
            const DrawIndirectDesc& drawDesc);

        /// <summary>
        /// Perform an indirect indexed draw call.
        /// </summary>
        void DrawIndirectIndexed(
            const DrawIndirectDesc& drawDesc);

        /// <summary>
        /// End rendering.
        /// </summary>
        void EndRendering();

    public:
        /// <summary>
        /// Dispatch compute operation
        /// </summary>
        void Dispatch(
            uint32_t x,
            uint32_t y = 1,
            uint32_t z = 1);

    public:
        /// <summary>
        /// Perform a copy from buffer to buffer
        /// Copy the rest of buffer if size is 0
        /// </summary>
        void CopyBuffer(
            const BufferCopyDesc& src,
            const BufferCopyDesc& dst,
            size_t                size = 0);

        /// <summary>
        /// Perform a copy from texture to texture
        /// </summary>
        void CopyTexture(
            const TextureCopyDesc& src,
            const TextureCopyDesc& dst);

        /// <summary>
        /// Perform a copy from buffer to texture
        /// </summary>
        void UploadTexture(
            const TransferCopyDesc& copyDesc);

        /// <summary>
        /// Perform a copy from texture to buffer
        /// </summary>
        void ReadbackTexture(
            const TransferCopyDesc& copyDesc);

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Buffer&      buffer,
            const AccessStage& state,
            bool               append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Texture&            texture,
            const AccessLayoutStage&  state,
            const TextureSubresource& subresource = AllSubresources,
            bool                      append      = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const GlobalBarrierDesc& barrierDesc);

        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers();

    public:
        /// <summary>
        /// Clear storage buffer
        /// </summary>
        void ClearBuffer(
            const ClearBufferDesc& clearDesc);

        /// <summary>
        /// Clear storage texture
        /// </summary>
        void ClearTexture(
            const ClearTextureDesc& clearDesc);

    private:
        nri::CommandAllocator* m_CommandAllocator = nullptr;
        nri::CommandBuffer*    m_CommandBuffer    = nullptr;
        DeviceImpl*            m_RhiDevice        = nullptr;
        Ptr<PipelineLayout>    m_PipelineLayout;
        DescriptorAllocator    m_DescriptorAllocator;
    };
} // namespace Ame::Rhi