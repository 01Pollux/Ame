#include <ranges>

#include <Rhi/CommandList/CommandListImpl.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>
#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/VertexView.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void CommandListImpl::Initialize(
        DeviceImpl&                     rhiDevice,
        const DescriptorAllocationDesc& allocationDesc,
        const char*                     allocatorName,
        const char*                     listName)
    {
        m_RhiDevice = &rhiDevice;

        auto& graphicsQueue = m_RhiDevice->GetQueue();
        auto& nriUtils      = m_RhiDevice->GetNRI();
        auto& nriCore       = *nriUtils.GetCoreInterface();

        ThrowIfFailed(nriCore.CreateCommandAllocator(graphicsQueue, m_CommandAllocator), "Failed to create command allocator");
        ThrowIfFailed(nriCore.CreateCommandBuffer(*m_CommandAllocator, m_CommandBuffer), "Failed to create command buffer");

#ifndef AME_DIST
        if (allocatorName)
        {
            nriCore.SetCommandAllocatorDebugName(*m_CommandAllocator, allocatorName);
        }
        if (listName)
        {
            nriCore.SetCommandBufferDebugName(*m_CommandBuffer, listName);
        }
#endif

        m_DescriptorAllocator.Initialize(*m_RhiDevice, allocationDesc);
    }

    void CommandListImpl::Shutdown()
    {
        m_DescriptorAllocator.Shutdown();

        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.DestroyCommandBuffer(*m_CommandBuffer);
        nriCore.DestroyCommandAllocator(*m_CommandAllocator);
    }

    void CommandListImpl::Reset()
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.ResetCommandAllocator(*m_CommandAllocator);
        nriCore.BeginCommandBuffer(*m_CommandBuffer, m_DescriptorAllocator.GetPool());

        m_DescriptorAllocator.ResetPool();
    }

    void CommandListImpl::End()
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.EndCommandBuffer(*m_CommandBuffer);
        m_PipelineLayout = nullptr;
    }

    //

    nri::CommandBuffer& CommandListImpl::GetCommandBuffer() noexcept
    {
        return *m_CommandBuffer;
    }

    void CommandListImpl::Submit()
    {
        auto& graphicsQueue = m_RhiDevice->GetQueue();
        auto& nriUtils      = m_RhiDevice->GetNRI();
        auto& nriCore       = *nriUtils.GetCoreInterface();

        nri::QueueSubmitDesc submitDesc{
            .commandBuffers   = &m_CommandBuffer,
            .commandBufferNum = 1
        };
        nriCore.QueueSubmit(graphicsQueue, submitDesc);
    }

    //

    void CommandListImpl::BeginMarker(
        const char* name)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdBeginAnnotation(*m_CommandBuffer, name);
    }

    void CommandListImpl::EndMarker()
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdEndAnnotation(*m_CommandBuffer);
    }

    //

    void CommandListImpl::SetPipelineLayout(
        const Ptr<PipelineLayout>& layout)
    {
        if (!m_PipelineLayout || m_PipelineLayout->GetHash() != layout->GetHash())
        {
            auto& nriUtils = m_RhiDevice->GetNRI();
            auto& nriCore  = *nriUtils.GetCoreInterface();

            nriCore.CmdSetPipelineLayout(*m_CommandBuffer, layout->Unwrap());
            m_PipelineLayout = layout;
        }
    }

    void CommandListImpl::SetPipelineState(
        const Ptr<PipelineState>& pipeline)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetPipeline(*m_CommandBuffer, pipeline->Unwrap());
    }

    //

    void CommandListImpl::SetConstants(
        uint32_t    constantIndex,
        const void* data,
        size_t      size)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetConstants(*m_CommandBuffer, constantIndex, data, size);
    }

    void CommandListImpl::SetDescriptorSet(
        uint32_t             layoutSlot,
        const DescriptorSet& descriptorSets,
        const uint32_t*      dynamicBufferOffset)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetDescriptorSet(*m_CommandBuffer, layoutSlot, *descriptorSets.Unwrap(), dynamicBufferOffset);
    }

    void CommandListImpl::SetSamplePositions(
        std::span<const SamplePosition> positions,
        Sample_t                        sampleCount)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetSamplePositions(*m_CommandBuffer, positions.data(), static_cast<Sample_t>(positions.size()), sampleCount);
    }

    //

    DescriptorSet CommandListImpl::AllocateSet(
        uint32_t layoutSlot,
        uint32_t variableCount)
    {
        return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), layoutSlot, variableCount);
    }

    std::vector<DescriptorSet> CommandListImpl::AllocateSets(
        uint32_t layoutSlot,
        uint32_t instanceCount,
        uint32_t variableCount)
    {
        return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), layoutSlot, instanceCount, variableCount);
    }

    //

    void CommandListImpl::BeginRendering(
        std::span<const Rhi::ResourceView*> renderTargets,
        const Rhi::ResourceView*            depthStencil)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        auto nriRenderTargets = renderTargets |
                                std::views::transform([](const Rhi::ResourceView* View)
                                                      { return View->Unwrap(); }) |
                                std::ranges::to<std::vector>();

        nri::AttachmentsDesc attachements{
            .depthStencil = depthStencil ? depthStencil->Unwrap() : nullptr,
            .colors       = nriRenderTargets.data(),
            .colorNum     = Count32(nriRenderTargets)
        };
        nriCore.CmdBeginRendering(*m_CommandBuffer, attachements);
    }

    void CommandListImpl::ClearAttachments(
        std::span<const ClearDesc>   clears,
        std::span<const ClearRegion> regions)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdClearAttachments(*m_CommandBuffer, clears.data(), Count32(clears), regions.data(), Count32(regions));
    }

    void CommandListImpl::ClearAttachments(
        std::span<const ClearDesc> clears)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdClearAttachments(*m_CommandBuffer, clears.data(), Count32(clears), nullptr, 0);
    }

    void CommandListImpl::SetViewports(
        std::span<const Viewport> viewports)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetViewports(*m_CommandBuffer, viewports.data(), Count32(viewports));
    }

    void CommandListImpl::SetScissorRects(
        std::span<const ScissorRect> scissorRects)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetScissors(*m_CommandBuffer, scissorRects.data(), Count32(scissorRects));
    }

    void CommandListImpl::SetStencilReference(
        uint8_t stencilReference)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetStencilReference(*m_CommandBuffer, stencilReference, stencilReference);
    }

    void CommandListImpl::SetDepthBounds(
        float minDepthBounds,
        float maxDepthBounds)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetDepthBounds(*m_CommandBuffer, minDepthBounds, maxDepthBounds);
    }

    void CommandListImpl::SetBlendConstants(
        const Math::Color4& blendConstants)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::Color32f NriColor{
            blendConstants.r,
            blendConstants.g,
            blendConstants.b,
            blendConstants.a
        };
        nriCore.CmdSetBlendConstants(*m_CommandBuffer, NriColor);
    }

    void CommandListImpl::SetVertexBuffers(
        std::span<const VertexBufferView> vertexBuffers,
        uint32_t                          baseSlot)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        std::vector<nri::Buffer*> nriBuffers;
        std::vector<uint64_t>     offsets;

        nriBuffers.reserve(vertexBuffers.size());
        offsets.reserve(vertexBuffers.size());

        for (const auto& vertexBuffer : vertexBuffers)
        {
            nriBuffers.push_back(vertexBuffer.Buffer);
            offsets.push_back(vertexBuffer.Offset);
        }

        nriCore.CmdSetVertexBuffers(*m_CommandBuffer, baseSlot, Count32(nriBuffers), nriBuffers.data(), offsets.data());
    }

    void CommandListImpl::SetIndexBuffer(
        const IndexBufferView& indexBuffer)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdSetIndexBuffer(*m_CommandBuffer, *indexBuffer.Buffer, indexBuffer.Offset, indexBuffer.Type);
    }

    void CommandListImpl::Draw(
        const DrawDesc& drawDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdDraw(*m_CommandBuffer, drawDesc);
    }

    void CommandListImpl::Draw(
        const DrawIndexedDesc& drawDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdDrawIndexed(*m_CommandBuffer, drawDesc);
    }

    void CommandListImpl::DrawIndirect(
        const DrawIndirectDesc& drawDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdDrawIndirect(
            *m_CommandBuffer,
            *drawDesc.DrawBuffer,
            drawDesc.DrawOffset,
            drawDesc.MaxDrawCount,
            m_RhiDevice->GetDrawIndexedCommandSize(),
            drawDesc.CounterBuffer,
            drawDesc.CounterOffset);
    }

    void CommandListImpl::DrawIndirectIndexed(
        const DrawIndirectDesc& drawDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdDrawIndexedIndirect(
            *m_CommandBuffer,
            *drawDesc.DrawBuffer,
            drawDesc.DrawOffset,
            drawDesc.MaxDrawCount,
            m_RhiDevice->GetDrawIndexedCommandSize(),
            drawDesc.CounterBuffer,
            drawDesc.CounterOffset);
    }

    void CommandListImpl::EndRendering()
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdEndRendering(*m_CommandBuffer);
    }

    //

    void CommandListImpl::Dispatch(
        uint32_t x,
        uint32_t y,
        uint32_t z)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdDispatch(*m_CommandBuffer, { x, y, z });
    }

    //

    void CommandListImpl::CopyBuffer(
        const BufferCopyDesc& src,
        const BufferCopyDesc& dst,
        size_t                size)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        if (!size)
        {
            auto& srcDesc = nriCore.GetBufferDesc(*src.RhiBuffer.get().Unwrap());
            auto& dstDesc = nriCore.GetBufferDesc(*dst.RhiBuffer.get().Unwrap());

            size = std::min(srcDesc.size - src.Offset, dstDesc.size - dst.Offset);
        }

        nriCore.CmdCopyBuffer(*m_CommandBuffer, *dst.RhiBuffer.get().Unwrap(), dst.Offset, *src.RhiBuffer.get().Unwrap(), src.Offset, size);
    }

    void CommandListImpl::CopyTexture(
        const TextureCopyDesc& src,
        const TextureCopyDesc& dst)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdCopyTexture(*m_CommandBuffer, *dst.RhiTexture.get().Unwrap(), dst.Region, *src.RhiTexture.get().Unwrap(), src.Region);
    }

    void CommandListImpl::UploadTexture(
        const TransferCopyDesc& copyDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdUploadBufferToTexture(*m_CommandBuffer, *copyDesc.RhiTexture.get().Unwrap(), copyDesc.Region, *copyDesc.RhiBuffer.get().Unwrap(), copyDesc.Layout);
    }

    void CommandListImpl::ReadbackTexture(
        const TransferCopyDesc& copyDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        auto copyLayout = copyDesc.Layout;
        nriCore.CmdReadbackTextureToBuffer(*m_CommandBuffer, *copyDesc.RhiBuffer.get().Unwrap(), copyLayout, *copyDesc.RhiTexture.get().Unwrap(), copyDesc.Region);
    }

    //

    AccessStage CommandListImpl::QueryState(
        const Buffer& buffer)
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();

        return stateTracker.QueryState(buffer.Unwrap());
    }

    Co::generator<AccessLayoutStage> CommandListImpl::QueryState(
        const Texture&            texture,
        const TextureSubresource& subresource)
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();

        auto copySubresource = subresource.Transform(texture);

        return stateTracker.QueryState(
            texture.Unwrap(),
            copySubresource.Mips.Offset,
            copySubresource.Mips.Count,
            copySubresource.Array.Offset,
            copySubresource.Array.Count);
    }

    //

    void CommandListImpl::RequireState(
        const Buffer&      buffer,
        const AccessStage& state,
        bool               append)
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();

        stateTracker.RequireState(
            buffer.Unwrap(),
            state,
            append);
    }

    void CommandListImpl::RequireState(
        const Texture&            texture,
        const AccessLayoutStage&  state,
        const TextureSubresource& subresource,
        bool                      append)
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();

        auto copySubresource = subresource.Transform(texture);

        stateTracker.RequireState(
            nriCore,
            texture.Unwrap(),
            state,
            copySubresource.Mips.Offset,
            copySubresource.Mips.Count,
            copySubresource.Array.Offset,
            copySubresource.Array.Count,
            append);
    }

    void CommandListImpl::PlaceBarrier(
        const GlobalBarrierDesc& BarrierDesc)
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();
    }

    void CommandListImpl::CommitBarriers()
    {
        auto& nriUtils     = m_RhiDevice->GetNRI();
        auto& nriCore      = *nriUtils.GetCoreInterface();
        auto& stateTracker = m_RhiDevice->GetStateTracker();

        stateTracker.CommitBarriers(nriCore, *m_CommandBuffer);
    }

    //

    void CommandListImpl::ClearBuffer(
        const ClearBufferDesc& clearDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdClearStorageBuffer(*m_CommandBuffer, clearDesc);
    }

    void CommandListImpl::ClearTexture(
        const ClearTextureDesc& clearDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CmdClearStorageTexture(*m_CommandBuffer, clearDesc);
    }
} // namespace Ame::Rhi