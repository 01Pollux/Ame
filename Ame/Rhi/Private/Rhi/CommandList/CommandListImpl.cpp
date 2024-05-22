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
        DeviceImpl&                     RhiDevice,
        const DescriptorAllocationDesc& DescriptorPoolDesc,
        const char*                     AllocatorName,
        const char*                     ListName)
    {
        m_RhiDevice = &RhiDevice;

        auto& GraphicsQueue = m_RhiDevice->GetQueue();
        auto& Nri           = m_RhiDevice->GetNRI();
        auto& NriCore       = *Nri.GetCoreInterface();

        ThrowIfFailed(NriCore.CreateCommandAllocator(GraphicsQueue, m_CommandAllocator), "Failed to create command allocator");
        ThrowIfFailed(NriCore.CreateCommandBuffer(*m_CommandAllocator, m_CommandBuffer), "Failed to create command buffer");

#ifndef AME_DIST
        if (AllocatorName)
        {
            NriCore.SetCommandAllocatorDebugName(*m_CommandAllocator, AllocatorName);
        }
        if (ListName)
        {
            NriCore.SetCommandBufferDebugName(*m_CommandBuffer, ListName);
        }
#endif

        m_DescriptorAllocator.Initialize(*m_RhiDevice, DescriptorPoolDesc);
    }

    void CommandListImpl::Shutdown()
    {
        m_DescriptorAllocator.Shutdown();

        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.DestroyCommandBuffer(*m_CommandBuffer);
        NriCore.DestroyCommandAllocator(*m_CommandAllocator);
    }

    void CommandListImpl::Reset()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.ResetCommandAllocator(*m_CommandAllocator);
        NriCore.BeginCommandBuffer(*m_CommandBuffer, m_DescriptorAllocator.GetPool());

        m_DescriptorAllocator.ResetPool();
    }

    void CommandListImpl::End()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.EndCommandBuffer(*m_CommandBuffer);
        m_PipelineLayout = nullptr;
    }

    //

    nri::CommandBuffer& CommandListImpl::GetCommandBuffer() noexcept
    {
        return *m_CommandBuffer;
    }

    void CommandListImpl::Submit()
    {
        auto& GraphicsQueue = m_RhiDevice->GetQueue();
        auto& Nri           = m_RhiDevice->GetNRI();
        auto& NriCore       = *Nri.GetCoreInterface();

        nri::QueueSubmitDesc SubmitDesc{
            .commandBuffers   = &m_CommandBuffer,
            .commandBufferNum = 1
        };
        NriCore.QueueSubmit(GraphicsQueue, SubmitDesc);
    }

    //

    void CommandListImpl::BeginMarker(
        const char* Name)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdBeginAnnotation(*m_CommandBuffer, Name);
    }

    void CommandListImpl::EndMarker()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdEndAnnotation(*m_CommandBuffer);
    }

    //

    void CommandListImpl::SetPipelineLayout(
        const Ptr<PipelineLayout>& Layout)
    {
        if (!m_PipelineLayout || m_PipelineLayout->GetHash() != Layout->GetHash())
        {
            auto& Nri     = m_RhiDevice->GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.CmdSetPipelineLayout(*m_CommandBuffer, Layout->Unwrap());
            m_PipelineLayout = Layout;
        }
    }

    void CommandListImpl::SetPipelineState(
        const Ptr<PipelineState>& Pipeline)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetPipeline(*m_CommandBuffer, Pipeline->Unwrap());
    }

    //

    void CommandListImpl::SetConstants(
        uint32_t    ConstantIndex,
        const void* Data,
        size_t      Size)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetConstants(*m_CommandBuffer, ConstantIndex, Data, Size);
    }

    void CommandListImpl::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets,
        const uint32_t*      DynamicBufferOffset)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetDescriptorSet(*m_CommandBuffer, LayoutSlot, *DescriptorSets.Unwrap(), DynamicBufferOffset);
    }

    void CommandListImpl::SetSamplePositions(
        std::span<SamplePosition> Positions,
        Sample_t                  SampleCount)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetSamplePositions(*m_CommandBuffer, Positions.data(), static_cast<Sample_t>(Positions.size()), SampleCount);
    }

    //

    DescriptorSet CommandListImpl::AllocateSet(
        uint32_t LayoutSlot,
        uint32_t VariableCount)
    {
        return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), LayoutSlot, VariableCount);
    }

    std::vector<DescriptorSet> CommandListImpl::AllocateSets(
        uint32_t LayoutSlot,
        uint32_t InstanceCount,
        uint32_t VariableCount)
    {
        return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), LayoutSlot, InstanceCount, VariableCount);
    }

    //

    void CommandListImpl::BeginRendering(
        std::span<const Rhi::ResourceView*> RenderTargets,
        const Rhi::ResourceView*            DepthStencil)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto NriRenderTargets = RenderTargets |
                                std::views::transform([](const Rhi::ResourceView* View)
                                                      { return View->Unwrap(); }) |
                                std::ranges::to<std::vector>();

        nri::AttachmentsDesc Attachements{
            .depthStencil = DepthStencil ? DepthStencil->Unwrap() : nullptr,
            .colors       = NriRenderTargets.data(),
            .colorNum     = Count32(NriRenderTargets)
        };
        NriCore.CmdBeginRendering(*m_CommandBuffer, Attachements);
    }

    void CommandListImpl::ClearAttachments(
        std::span<ClearDesc>   Clears,
        std::span<ClearRegion> Regions)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdClearAttachments(*m_CommandBuffer, Clears.data(), Count32(Clears), Regions.data(), Count32(Regions));
    }

    void CommandListImpl::ClearAttachments(
        std::span<ClearDesc> Clears)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdClearAttachments(*m_CommandBuffer, Clears.data(), Count32(Clears), nullptr, 0);
    }

    void CommandListImpl::SetViewports(
        std::span<Viewport> Viewports)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetViewports(*m_CommandBuffer, Viewports.data(), Count32(Viewports));
    }

    void CommandListImpl::SetScissorRects(
        std::span<ScissorRect> ScissorRects)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetScissors(*m_CommandBuffer, ScissorRects.data(), Count32(ScissorRects));
    }

    void CommandListImpl::SetStencilReference(
        uint8_t StencilReference)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetStencilReference(*m_CommandBuffer, StencilReference, StencilReference);
    }

    void CommandListImpl::SetDepthBounds(
        float MinDepthBounds,
        float MaxDepthBounds)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetDepthBounds(*m_CommandBuffer, MinDepthBounds, MaxDepthBounds);
    }

    void CommandListImpl::SetBlendConstants(
        const Math::Color4& BlendConstants)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::Color32f NriColor{
            BlendConstants.r,
            BlendConstants.g,
            BlendConstants.b,
            BlendConstants.a
        };
        NriCore.CmdSetBlendConstants(*m_CommandBuffer, NriColor);
    }

    void CommandListImpl::SetVertexBuffers(
        std::span<const VertexBufferView> VertexBuffers,
        uint32_t                          BaseSlot)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        std::vector<nri::Buffer*> NriBuffers;
        std::vector<uint64_t>     Offsets;

        NriBuffers.reserve(VertexBuffers.size());
        Offsets.reserve(VertexBuffers.size());

        for (const auto& VertexBuffer : VertexBuffers)
        {
            NriBuffers.push_back(VertexBuffer.Buffer);
            Offsets.push_back(VertexBuffer.Offset);
        }

        NriCore.CmdSetVertexBuffers(*m_CommandBuffer, BaseSlot, Count32(NriBuffers), NriBuffers.data(), Offsets.data());
    }

    void CommandListImpl::SetIndexBuffer(
        const IndexBufferView& IndexBuffer)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetIndexBuffer(*m_CommandBuffer, *IndexBuffer.Buffer, IndexBuffer.Offset, IndexBuffer.Type);
    }

    void CommandListImpl::Draw(
        const DrawDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDraw(*m_CommandBuffer, Desc);
    }

    void CommandListImpl::Draw(
        const DrawIndexedDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDrawIndexed(*m_CommandBuffer, Desc);
    }

    void CommandListImpl::DrawIndirect(
        const DrawIndirectDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDrawIndirect(*m_CommandBuffer, *Desc.DrawBuffer, Desc.DrawOffset, Desc.MaxDrawCount, m_RhiDevice->GetDrawIndexedCommandSize(), Desc.CounterBuffer, Desc.CounterOffset);
    }

    void CommandListImpl::DrawIndirectIndexed(
        const DrawIndirectDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDrawIndexedIndirect(*m_CommandBuffer, *Desc.DrawBuffer, Desc.DrawOffset, Desc.MaxDrawCount, m_RhiDevice->GetDrawIndexedCommandSize(), Desc.CounterBuffer, Desc.CounterOffset);
    }

    void CommandListImpl::EndRendering()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdEndRendering(*m_CommandBuffer);
    }

    //

    void CommandListImpl::Dispatch(
        uint32_t X,
        uint32_t Y,
        uint32_t Z)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDispatch(*m_CommandBuffer, { .x = X, .y = Y, .z = Z });
    }

    //

    void CommandListImpl::CopyBuffer(
        const BufferCopyDesc& Src,
        const BufferCopyDesc& Dst,
        size_t                Size)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        if (!Size)
        {
            auto& SrcDesc = NriCore.GetBufferDesc(*Src.RhiBuffer.Unwrap());
            auto& DstDesc = NriCore.GetBufferDesc(*Dst.RhiBuffer.Unwrap());

            Size = std::min(SrcDesc.size - Src.Offset, DstDesc.size - Dst.Offset);
        }

        NriCore.CmdCopyBuffer(*m_CommandBuffer, *Dst.RhiBuffer.Unwrap(), Dst.Offset, *Src.RhiBuffer.Unwrap(), Src.Offset, Size);
    }

    void CommandListImpl::CopyTexture(
        const TextureCopyDesc& Src,
        const TextureCopyDesc& Dst)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdCopyTexture(*m_CommandBuffer, *Dst.RhiTexture.Unwrap(), Dst.Region, *Src.RhiTexture.Unwrap(), Src.Region);
    }

    void CommandListImpl::UploadTexture(
        const TransferCopyDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdUploadBufferToTexture(*m_CommandBuffer, *Desc.RhiTexture.Unwrap(), Desc.Region, *Desc.RhiBuffer.Unwrap(), Desc.Layout);
    }

    void CommandListImpl::ReadbackTexture(
        const TransferCopyDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto CopyLayout = Desc.Layout;
        NriCore.CmdReadbackTextureToBuffer(*m_CommandBuffer, *Desc.RhiBuffer.Unwrap(), CopyLayout, *Desc.RhiTexture.Unwrap(), Desc.Region);
    }

    //

    void CommandListImpl::RequireState(
        const Buffer&      RhiBuffer,
        const AccessStage& State,
        bool               Append)
    {
        auto& Nri          = m_RhiDevice->GetNRI();
        auto& NriCore      = *Nri.GetCoreInterface();
        auto& StateTracker = m_RhiDevice->GetStateTracker();

        StateTracker.RequireState(
            RhiBuffer.Unwrap(),
            State,
            Append);
    }

    void CommandListImpl::RequireState(
        const Texture&            RhiTexture,
        const AccessLayoutStage&  State,
        const TextureSubresource& Subresource,
        bool                      Append)
    {
        auto& Nri          = m_RhiDevice->GetNRI();
        auto& NriCore      = *Nri.GetCoreInterface();
        auto& StateTracker = m_RhiDevice->GetStateTracker();

        auto CopySubresource = Subresource.Transform(RhiTexture);

        StateTracker.RequireState(
            NriCore,
            RhiTexture.Unwrap(),
            State,
            CopySubresource.Mips.Offset,
            CopySubresource.Mips.Count,
            CopySubresource.Array.Offset,
            CopySubresource.Array.Count,
            Append);
    }

    void CommandListImpl::PlaceBarrier(
        const GlobalBarrierDesc& BarrierDesc)
    {
        auto& Nri          = m_RhiDevice->GetNRI();
        auto& NriCore      = *Nri.GetCoreInterface();
        auto& StateTracker = m_RhiDevice->GetStateTracker();
    }

    void CommandListImpl::CommitBarriers()
    {
        auto& Nri          = m_RhiDevice->GetNRI();
        auto& NriCore      = *Nri.GetCoreInterface();
        auto& StateTracker = m_RhiDevice->GetStateTracker();

        StateTracker.CommitBarriers(NriCore, *m_CommandBuffer);
    }

    //

    void CommandListImpl::ClearBuffer(
        const ClearBufferDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdClearStorageBuffer(*m_CommandBuffer, Desc);
    }

    void CommandListImpl::ClearTexture(
        const ClearTextureDesc& Desc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdClearStorageTexture(*m_CommandBuffer, Desc);
    }
} // namespace Ame::Rhi