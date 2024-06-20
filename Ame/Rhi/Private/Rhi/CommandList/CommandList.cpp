#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>

#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    void CommandList::SetName(
        const char* name) const
    {
        m_NriCore->SetCommandBufferDebugName(*m_CommandBuffer, name);
    }

    nri::CommandBuffer* const& CommandList::Unwrap() const noexcept
    {
        return m_CommandBuffer;
    }

    //

    void CommandList::BeginMarker(
        const char* name)
    {
        m_NriCore->CmdBeginAnnotation(*m_CommandBuffer, name);
    }

    void CommandList::EndMarker()
    {
        m_NriCore->CmdEndAnnotation(*m_CommandBuffer);
    }

    //

    void CommandList::ResourceBarrier(
        const BarrierGroupDesc& barrierGroup)
    {
        m_NriCore->CmdBarrier(*m_CommandBuffer, barrierGroup);
    }

    //

    void CommandList::SetPipelineLayout(
        const Ptr<PipelineLayout>& layout)
    {
        /*    if (!m_PipelineLayout || m_PipelineLayout->GetHash() != layout->GetHash())
            {
                m_NriCore->CmdSetPipelineLayout(*m_CommandBuffer, layout->Unwrap());
                m_PipelineLayout = layout;
            }*/
    }

    void CommandList::SetPipelineState(
        const Ptr<PipelineState>& pipeline)
    {
        /*
         m_NriCore->CmdSetPipeline(*m_CommandBuffer, pipeline->Unwrap());*/
    }

    //

    void CommandList::SetConstants(
        uint32_t    constantIndex,
        const void* data,
        size_t      size)
    {
        m_NriCore->CmdSetConstants(*m_CommandBuffer, constantIndex, data, size);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             layoutSlot,
        const DescriptorSet& descriptorSets,
        const uint32_t*      dynamicBufferOffset)
    {
        // m_NriCore->CmdSetDescriptorSet(*m_CommandBuffer, layoutSlot, *descriptorSets.Unwrap(), dynamicBufferOffset);
    }

    void CommandList::SetSamplePositions(
        std::span<const SamplePosition> positions,
        Sample_t                        sampleCount)
    {
        // m_NriCore->CmdSetSamplePositions(*m_CommandBuffer, positions.data(), static_cast<Sample_t>(positions.size()), sampleCount);
    }

    //

    // DescriptorSet CommandList::AllocateSet(
    //     uint32_t layoutSlot,
    //     uint32_t variableCount)
    //{
    //     return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), layoutSlot, variableCount);
    // }

    // std::vector<DescriptorSet> CommandList::AllocateSets(
    //     uint32_t layoutSlot,
    //     uint32_t instanceCount,
    //     uint32_t variableCount)
    //{
    //     return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), layoutSlot, instanceCount, variableCount);
    // }

    //

    void CommandList::BeginRendering(
        std::span<const Rhi::ResourceView*> renderTargets,
        const Rhi::ResourceView*            depthStencil)
    {
        auto nriRenderTargets = renderTargets |
                                std::views::transform([](const Rhi::ResourceView* View)
                                                      { return View->Unwrap(); }) |
                                std::ranges::to<std::vector>();

        nri::AttachmentsDesc attachements{
            .depthStencil = depthStencil ? depthStencil->Unwrap() : nullptr,
            .colors       = nriRenderTargets.data(),
            .colorNum     = Count32(nriRenderTargets)
        };
        m_NriCore->CmdBeginRendering(*m_CommandBuffer, attachements);
    }

    void CommandList::ClearAttachments(
        std::span<const ClearDesc>   clears,
        std::span<const ClearRegion> regions)
    {
        m_NriCore->CmdClearAttachments(*m_CommandBuffer, clears.data(), Count32(clears), regions.data(), Count32(regions));
    }

    void CommandList::ClearAttachments(
        std::span<const ClearDesc> clears)
    {
        m_NriCore->CmdClearAttachments(*m_CommandBuffer, clears.data(), Count32(clears), nullptr, 0);
    }

    void CommandList::SetViewports(
        std::span<const Viewport> viewports)
    {
        m_NriCore->CmdSetViewports(*m_CommandBuffer, viewports.data(), Count32(viewports));
    }

    void CommandList::SetScissorRects(
        std::span<const ScissorRect> scissorRects)
    {
        m_NriCore->CmdSetScissors(*m_CommandBuffer, scissorRects.data(), Count32(scissorRects));
    }

    void CommandList::SetStencilReference(
        uint8_t stencilReference)
    {
        m_NriCore->CmdSetStencilReference(*m_CommandBuffer, stencilReference, stencilReference);
    }

    void CommandList::SetDepthBounds(
        float minDepthBounds,
        float maxDepthBounds)
    {
        m_NriCore->CmdSetDepthBounds(*m_CommandBuffer, minDepthBounds, maxDepthBounds);
    }

    void CommandList::SetBlendConstants(
        const Math::Color4& blendConstants)
    {
        nri::Color32f NriColor{
            blendConstants.r(),
            blendConstants.g(),
            blendConstants.b(),
            blendConstants.a()
        };
        m_NriCore->CmdSetBlendConstants(*m_CommandBuffer, NriColor);
    }

    void CommandList::SetVertexBuffers(
        std::span<const VertexBufferView> vertexBuffers,
        uint32_t                          baseSlot)
    {
        std::vector<nri::Buffer*> nriBuffers;
        std::vector<uint64_t>     offsets;

        nriBuffers.reserve(vertexBuffers.size());
        offsets.reserve(vertexBuffers.size());

        for (const auto& vertexBuffer : vertexBuffers)
        {
            nriBuffers.push_back(vertexBuffer.NriBuffer);
            offsets.push_back(vertexBuffer.Offset);
        }

        m_NriCore->CmdSetVertexBuffers(*m_CommandBuffer, baseSlot, Count32(nriBuffers), nriBuffers.data(), offsets.data());
    }

    void CommandList::SetIndexBuffer(
        const IndexBufferView& indexBuffer)
    {
        m_NriCore->CmdSetIndexBuffer(*m_CommandBuffer, *indexBuffer.NriBuffer, indexBuffer.Offset, indexBuffer.Type);
    }

    void CommandList::Draw(
        const DrawDesc& drawDesc)
    {
        m_NriCore->CmdDraw(*m_CommandBuffer, drawDesc);
    }

    void CommandList::Draw(
        const DrawIndexedDesc& drawDesc)
    {
        m_NriCore->CmdDrawIndexed(*m_CommandBuffer, drawDesc);
    }

    void CommandList::DrawIndirect(
        const DrawIndirectDesc& drawDesc)
    {
        // m_NriCore->CmdDrawIndirect(
        //     *m_CommandBuffer,
        //     *drawDesc.DrawBuffer,
        //     drawDesc.DrawOffset,
        //     drawDesc.MaxDrawCount,
        //     m_RhiDevice->GetDrawIndexedCommandSize(),
        //     drawDesc.CounterBuffer,
        //     drawDesc.CounterOffset);
    }

    void CommandList::DrawIndirectIndexed(
        const DrawIndirectDesc& drawDesc)
    {
        /*       m_NriCore->CmdDrawIndexedIndirect(
                   *m_CommandBuffer,
                   *drawDesc.DrawBuffer,
                   drawDesc.DrawOffset,
                   drawDesc.MaxDrawCount,
                   m_RhiDevice->GetDrawIndexedCommandSize(),
                   drawDesc.CounterBuffer,
                   drawDesc.CounterOffset);*/
    }

    void CommandList::EndRendering()
    {
        m_NriCore->CmdEndRendering(*m_CommandBuffer);
    }

    //

    void CommandList::Dispatch(
        uint32_t x,
        uint32_t y,
        uint32_t z)
    {
        m_NriCore->CmdDispatch(*m_CommandBuffer, { x, y, z });
    }

    //

    void CommandList::CopyBuffer(
        const BufferCopyDesc& src,
        const BufferCopyDesc& dst,
        size_t                size)
    {
        if (!size)
        {
            auto& srcDesc = m_NriCore->GetBufferDesc(*src.NriBuffer);
            auto& dstDesc = m_NriCore->GetBufferDesc(*dst.NriBuffer);

            size = std::min(srcDesc.size - src.Offset, dstDesc.size - dst.Offset);
        }

        m_NriCore->CmdCopyBuffer(*m_CommandBuffer, *dst.NriBuffer, dst.Offset, *src.NriBuffer, src.Offset, size);
    }

    void CommandList::CopyTexture(
        const TextureCopyDesc& src,
        const TextureCopyDesc& dst)
    {
        const nri::TextureRegionDesc* dstRegion = nullptr;
        const nri::TextureRegionDesc* srcRegion = nullptr;

        if (dst.Region)
        {
            dstRegion = &dst.Region.value();
        }
        if (src.Region)
        {
            srcRegion = &src.Region.value();
        }

        m_NriCore->CmdCopyTexture(*m_CommandBuffer, *dst.NriTexture, dstRegion, *src.NriTexture, srcRegion);
    }

    void CommandList::UploadTexture(
        const TransferCopyDesc& copyDesc)
    {
        // auto& textureDesc = m_NriCore->GetTextureDesc(*copyDesc.NriTexture);

        // nri::TextureDataLayoutDesc layout{
        //     .offset     = copyDesc.BufferOffset,
        //     .rowPitch   = Util::GetUploadBufferTextureRowSize(deviceDesc, textureDesc.format, copyDesc.TextureRegion.width),
        //     .slicePitch = Util::GetUploadBufferTextureSliceSize(deviceDesc, textureDesc.format, layout.rowPitch, copyDesc.TextureRegion.height)
        // };
        // m_NriCore->CmdUploadBufferToTexture(*m_CommandBuffer, *copyDesc.NriTexture, copyDesc.TextureRegion, *copyDesc.NriBuffer, layout);
    }

    void CommandList::ReadbackTexture(
        const TransferCopyDesc& copyDesc)
    {
        // auto& textureDesc = m_NriCore->GetTextureDesc(*copyDesc.NriTexture);

        // nri::TextureDataLayoutDesc layout{
        //     .offset     = copyDesc.BufferOffset,
        //     .rowPitch   = Util::GetUploadBufferTextureRowSize(deviceDesc, textureDesc.format, copyDesc.TextureRegion.width),
        //     .slicePitch = Util::GetUploadBufferTextureSliceSize(deviceDesc, textureDesc.format, layout.rowPitch, copyDesc.TextureRegion.height)
        // };
        // m_NriCore->CmdReadbackTextureToBuffer(*m_CommandBuffer, *copyDesc.NriBuffer, layout, *copyDesc.NriTexture, copyDesc.TextureRegion);
    }

    //

    void CommandList::ClearBuffer(
        const ClearBufferDesc& clearDesc)
    {
        m_NriCore->CmdClearStorageBuffer(*m_CommandBuffer, clearDesc);
    }

    void CommandList::ClearTexture(
        const ClearTextureDesc& clearDesc)
    {
        m_NriCore->CmdClearStorageTexture(*m_CommandBuffer, clearDesc);
    }
} // namespace Ame::Rhi