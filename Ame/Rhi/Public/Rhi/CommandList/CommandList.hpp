#pragma once

#include <Rhi/Descs/CommandList.hpp>
#include <Rhi/CommandList/CopyDesc.hpp>
#include <Rhi/Resource/VertexView.hpp>

namespace Ame::Rhi
{
    class CommandList
    {
    public:
        CommandList() = default;
        CommandList(
            nri::CoreInterface* nriCore,
            nri::CommandBuffer* commandBuffer) noexcept :
            m_NriCore(nriCore),
            m_CommandBuffer(commandBuffer)
        {
        }

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri command list.
        /// </summary>
        [[nodiscard]] nri::CommandBuffer* const& Unwrap() const noexcept;

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
        /// Place a barrier in the command buffer.
        /// </summary>
        void ResourceBarrier(
            const BarrierGroupDesc& barrierGroup);

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
        /// Set constants.
        /// </summary>
        template<typename Ty>
            requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
        void SetConstants(
            uint32_t  constantIndex,
            const Ty& data)
        {
            SetConstants(constantIndex, &data, sizeof(Ty));
        }

        /// <summary>
        /// Set descriptor sets with dynamic offsets if not null.
        /// </summary>
        void SetDescriptorSet(
            uint32_t             layoutSlot,
            const DescriptorSet& descriptorSets,
            const uint32_t*      dynamicBufferOffset = nullptr);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<const SamplePosition> positions,
            Sample_t                        sampleCount);

        // public:
        //     /// <summary>
        //     /// Allocate descriptor sets for the pipeline layout.
        //     /// </summary>
        //     [[nodiscard]] DescriptorSet AllocateSet(
        //         uint32_t layoutSlot,
        //         uint32_t variableCount = 0);

        //    /// <summary>
        //    /// Allocate descriptor sets for the pipeline layout.
        //    /// </summary>
        //    [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
        //        uint32_t layoutSlot,
        //        uint32_t instanceCount = 1,
        //        uint32_t variableCount = 0);

    public:
        /// <summary>
        /// Set render targets.
        /// </summary>
        void BeginRendering(
            std::span<const Rhi::ResourceView*> renderTargets,
            const Rhi::ResourceView*            depthStencil = nullptr);

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
        /// Set viewport of the render target.
        /// </summary>
        void SetViewport(
            const Viewport& viewport);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            std::span<const ScissorRect> scissorRects);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRect(
            const ScissorRect& scissorRects);

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
        /// Set vertex buffers.
        /// </summary>
        void SetVertexBuffers(
            std::span<const VertexBufferView> vertexBuffers,
            uint32_t                          baseSlot = 0);

        /// <summary>
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffer(
            const VertexBufferView& vertexBuffer,
            uint32_t                baseSlot = 0);

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
        nri::CoreInterface* m_NriCore       = nullptr;
        nri::CommandBuffer* m_CommandBuffer = nullptr;
    };

    //

    class MarkerCommand
    {
    public:
        MarkerCommand(
            CommandList& commandList,
            const char*  tag) :
            m_CommandList(commandList)
        {
#ifndef AME_DIST
            m_CommandList.get().BeginMarker(tag);
#endif
        }

        MarkerCommand(const MarkerCommand&)            = delete;
        MarkerCommand& operator=(const MarkerCommand&) = delete;

        MarkerCommand(MarkerCommand&&)            = delete;
        MarkerCommand& operator=(MarkerCommand&&) = delete;

        ~MarkerCommand()
        {
#ifndef AME_DIST
            m_CommandList.get().EndMarker();
#endif
        }

    private:
        Ref<CommandList> m_CommandList;
    };

    //

    class RenderingCommand
    {
    public:
        RenderingCommand(
            CommandList&                        commandList,
            std::span<const Rhi::ResourceView*> renderTargets,
            const Rhi::ResourceView*            depthStencil = nullptr) :
            m_CommandList(commandList)
        {
            m_CommandList.get().BeginRendering(renderTargets, depthStencil);
        }

        RenderingCommand(const RenderingCommand&)            = delete;
        RenderingCommand& operator=(const RenderingCommand&) = delete;

        RenderingCommand(RenderingCommand&&)            = delete;
        RenderingCommand& operator=(RenderingCommand&&) = delete;

        ~RenderingCommand()
        {
            m_CommandList.get().EndRendering();
        }

    private:
        Ref<CommandList> m_CommandList;
    };
} // namespace Ame::Rhi