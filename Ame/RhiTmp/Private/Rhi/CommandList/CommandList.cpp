#include <Rhi/CommandList/CommandListImpl.hpp>
#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/Resource/VertexView.hpp>

namespace Ame::Rhi
{
    CommandList::CommandList(
        Device& rhiDevice) :
        m_Impl(rhiDevice.GetImpl().GetCurrentCommandList())
    {
    }

    void CommandList::BeginMarker(
        const char* name)
    {
        m_Impl.get().BeginMarker(name);
    }

    void CommandList::EndMarker()
    {
        m_Impl.get().EndMarker();
    }

    //

    void CommandList::SetPipelineLayout(
        const Ptr<PipelineLayout>& layout)
    {
        m_Impl.get().SetPipelineLayout(layout);
    }

    void CommandList::SetPipelineState(
        const Ptr<PipelineState>& pipeline)
    {
        m_Impl.get().SetPipelineState(pipeline);
    }

    //

    void CommandList::SetConstants(
        uint32_t    constantIndex,
        const void* data,
        size_t      size)
    {
        m_Impl.get().SetConstants(constantIndex, data, size);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             layoutSlot,
        const DescriptorSet& descriptorSets,
        const uint32_t*      dynamicBufferOffset)
    {
        m_Impl.get().SetDescriptorSet(layoutSlot, descriptorSets, dynamicBufferOffset);
    }

    void CommandList::SetSamplePositions(
        std::span<const SamplePosition> positions,
        Sample_t                        sampleCount)
    {
        m_Impl.get().SetSamplePositions(positions, sampleCount);
    }

    //

    DescriptorSet CommandList::AllocateSet(
        uint32_t layoutSlot,
        uint32_t variableCount)
    {
        return m_Impl.get().AllocateSet(layoutSlot, variableCount);
    }

    std::vector<DescriptorSet> CommandList::AllocateSets(
        uint32_t layoutSlot,
        uint32_t instanceCount,
        uint32_t variableCount)
    {
        return m_Impl.get().AllocateSets(layoutSlot, instanceCount, variableCount);
    }

    //

    void CommandList::BeginRendering(
        std::span<const Rhi::ResourceView*> renderTargets,
        const Rhi::ResourceView*            depthStencil)
    {
        m_Impl.get().BeginRendering(renderTargets, depthStencil);
    }

    void CommandList::ClearAttachments(
        std::span<const ClearDesc>   clears,
        std::span<const ClearRegion> regions)
    {
        m_Impl.get().ClearAttachments(clears, regions);
    }

    void CommandList::ClearAttachments(
        std::span<const ClearDesc> clears)
    {
        m_Impl.get().ClearAttachments(clears);
    }

    void CommandList::SetViewports(
        std::span<const Viewport> viewports)
    {
        m_Impl.get().SetViewports(viewports);
    }

    void CommandList::SetViewport(
        const Viewport& viewport)
    {
        SetViewports({ &viewport, 1 });
    }

    void CommandList::SetScissorRects(
        std::span<const ScissorRect> scissorRects)
    {
        m_Impl.get().SetScissorRects(scissorRects);
    }

    void CommandList::SetScissorRect(
        const ScissorRect& scissorRects)
    {
        SetScissorRects({ &scissorRects, 1 });
    }

    void CommandList::SetStencilReference(
        uint8_t stencilReference)
    {
        m_Impl.get().SetStencilReference(stencilReference);
    }

    void CommandList::SetDepthBounds(
        float minDepthBounds,
        float maxDepthBounds)
    {
        m_Impl.get().SetDepthBounds(minDepthBounds, maxDepthBounds);
    }

    void CommandList::SetBlendConstants(
        const Math::Color4& blendConstants)
    {
        m_Impl.get().SetBlendConstants(blendConstants);
    }

    void CommandList::SetVertexBuffers(
        std::span<const VertexBufferView> vertexBuffers,
        uint32_t                          baseSlot)
    {
        m_Impl.get().SetVertexBuffers(vertexBuffers, baseSlot);
    }

    void CommandList::SetVertexBuffer(
        const VertexBufferView& vertexBuffer,
        uint32_t                baseSlot)
    {
        SetVertexBuffers({ &vertexBuffer, 1 }, baseSlot);
    }

    void CommandList::SetIndexBuffer(
        const IndexBufferView& indexBuffer)
    {
        m_Impl.get().SetIndexBuffer(indexBuffer);
    }

    void CommandList::Draw(
        const DrawDesc& drawDesc)
    {
        m_Impl.get().Draw(drawDesc);
    }

    void CommandList::Draw(
        const DrawIndexedDesc& drawDesc)
    {
        m_Impl.get().Draw(drawDesc);
    }

    void CommandList::DrawIndirect(
        const DrawIndirectDesc& drawDesc)
    {
        m_Impl.get().DrawIndirect(drawDesc);
    }

    void CommandList::DrawIndirectIndexed(
        const DrawIndirectDesc& drawDesc)
    {
        m_Impl.get().DrawIndirectIndexed(drawDesc);
    }

    void CommandList::EndRendering()
    {
        m_Impl.get().EndRendering();
    }

    //

    void CommandList::Dispatch(
        uint32_t x,
        uint32_t y,
        uint32_t z)
    {
        m_Impl.get().Dispatch(x, y, z);
    }

    //

    void CommandList::CopyBuffer(
        const BufferCopyDesc& src,
        const BufferCopyDesc& dst,
        size_t                size)
    {
        m_Impl.get().CopyBuffer(src, dst, size);
    }

    void CommandList::CopyTexture(
        const TextureCopyDesc& src,
        const TextureCopyDesc& dst)
    {
        m_Impl.get().CopyTexture(src, dst);
    }

    void CommandList::UploadTexture(
        const TransferCopyDesc& copyDesc)
    {
        m_Impl.get().UploadTexture(copyDesc);
    }

    void CommandList::ReadbackTexture(
        const TransferCopyDesc& copyDesc)
    {
        m_Impl.get().ReadbackTexture(copyDesc);
    }

    //

    AccessStage CommandList::QueryState(
        nri::Buffer* nribuffer)
    {
        return m_Impl.get().QueryState(nribuffer);
    }

    Co::generator<AccessLayoutStage> CommandList::QueryState(
        nri::Texture*             nriTexture,
        const TextureSubresource& subresource)
    {
        return m_Impl.get().QueryState(nriTexture, subresource);
    }

    //

    void CommandList::RequireState(
        nri::Buffer*       nribuffer,
        const AccessStage& state,
        bool               append)
    {
        m_Impl.get().RequireState(nribuffer, state, append);
    }

    void CommandList::RequireState(
        nri::Texture*             nriTexture,
        const AccessLayoutStage&  state,
        const TextureSubresource& subresource,
        bool                      append)
    {
        m_Impl.get().RequireState(nriTexture, state, subresource, append);
    }

    void CommandList::RequireStates(
        nri::Texture*                      nriTexture,
        std::span<const AccessLayoutStage> states,
        bool                               append)
    {
        m_Impl.get().RequireStates(nriTexture, states, append);
    }

    void CommandList::PlaceBarrier(
        const GlobalBarrierDesc& barrierDesc)
    {
        m_Impl.get().PlaceBarrier(barrierDesc);
    }

    void CommandList::CommitBarriers()
    {
        m_Impl.get().CommitBarriers();
    }

    //

    void CommandList::ClearBuffer(
        const ClearBufferDesc& clearDesc)
    {
        m_Impl.get().ClearBuffer(clearDesc);
    }

    void CommandList::ClearTexture(
        const ClearTextureDesc& clearDesc)
    {
        m_Impl.get().ClearTexture(clearDesc);
    }
} // namespace Ame::Rhi