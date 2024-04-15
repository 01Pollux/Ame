#include <ranges>

#include <Rhi/Resource/CommandListImpl.hpp>
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

    void CommandListImpl::SetPipelineLayout(
        const Ptr<PipelineLayout>& Layout)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetPipelineLayout(*m_CommandBuffer, Layout->Unwrap());
        m_PipelineLayout = Layout;
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
        uint32_t*            DynamicBufferOffset)
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

    std::vector<DescriptorSet> CommandListImpl::AllocateSets(
        uint32_t LayoutSlot,
        uint32_t InstanceCount,
        uint32_t VariableCount)
    {
        return m_DescriptorAllocator.Allocate(m_PipelineLayout->Unwrap(), LayoutSlot, InstanceCount, VariableCount);
    }

    //

    void CommandListImpl::BeginRendering(
        std::span<Rhi::ResourceView> RenderTargets,
        Rhi::ResourceView            DepthStencil)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto NriRenderTargets = RenderTargets |
                                std::views::transform([](const Rhi::ResourceView& View)
                                                      { return View.Unwrap(); }) |
                                std::ranges::to<std::vector>();

        nri::AttachmentsDesc Attachements{
            .depthStencil = DepthStencil.Unwrap(),
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
            NriBuffers.push_back(VertexBuffer.Buffer.Unwrap());
            Offsets.push_back(VertexBuffer.Offset);
        }

        NriCore.CmdSetVertexBuffers(*m_CommandBuffer, BaseSlot, Count32(NriBuffers), NriBuffers.data(), Offsets.data());
    }

    void CommandListImpl::SetIndexBuffer(
        const IndexBufferView& IndexBuffer)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetIndexBuffer(*m_CommandBuffer, *IndexBuffer.Buffer.Unwrap(), IndexBuffer.Offset, IndexBuffer.Type);
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

    void CommandListImpl::EndRendering()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdEndRendering(*m_CommandBuffer);
    }
} // namespace Ame::Rhi