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
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue,
        const char*         AllocatorName,
        const char*         ListName)
    {
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
    }

    void CommandListImpl::Shutdown(
        nri::CoreInterface& NriCore)
    {
        NriCore.DestroyCommandBuffer(*m_CommandBuffer);
        NriCore.DestroyCommandAllocator(*m_CommandAllocator);
    }

    void CommandListImpl::Reset(
        nri::CoreInterface& NriCore)
    {
        NriCore.ResetCommandAllocator(*m_CommandAllocator);
        NriCore.BeginCommandBuffer(*m_CommandBuffer, nullptr);
    }

    void CommandListImpl::End(
        nri::CoreInterface& NriCore)
    {
        NriCore.EndCommandBuffer(*m_CommandBuffer);
    }

    //

    nri::CommandBuffer& CommandListImpl::GetCommandBuffer() noexcept
    {
        return *m_CommandBuffer;
    }

    void CommandListImpl::Submit(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue)
    {
        nri::QueueSubmitDesc SubmitDesc{
            .commandBuffers   = &m_CommandBuffer,
            .commandBufferNum = 1
        };
        NriCore.QueueSubmit(GraphicsQueue, SubmitDesc);
    }

    //

    void CommandListImpl::SetPipelineLayout(
        DeviceImpl&     RhiDevice,
        PipelineLayout& Layout)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetPipelineLayout(*m_CommandBuffer, Layout.Unwrap());
    }

    void CommandListImpl::SetPipelineState(
        DeviceImpl&    RhiDevice,
        PipelineState& Pipeline)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetPipeline(*m_CommandBuffer, Pipeline.Unwrap());
    }

    //

    void CommandListImpl::SetConstants(
        DeviceImpl& RhiDevice,
        uint32_t    ConstantIndex,
        const void* Data,
        size_t      Size)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetConstants(*m_CommandBuffer, ConstantIndex, Data, Size);
    }

    void CommandListImpl::SetDescriptorSet(
        DeviceImpl&          RhiDevice,
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets,
        uint32_t*            DynamicBufferOffset)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetDescriptorSet(*m_CommandBuffer, LayoutSlot, *DescriptorSets.Unwrap(), DynamicBufferOffset);
    }

    void CommandListImpl::SetSamplePositions(
        DeviceImpl&               RhiDevice,
        std::span<SamplePosition> Positions,
        Sample_t                  SampleCount)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetSamplePositions(*m_CommandBuffer, Positions.data(), static_cast<Sample_t>(Positions.size()), SampleCount);
    }

    //

    std::vector<DescriptorSet*> CommandListImpl::AllocateSets(
        DeviceImpl& RhiDevice,
        uint32_t    Count)
    {
        return std::vector<DescriptorSet*>();
    }

    //

    void CommandListImpl::BeginRendering(
        DeviceImpl&                  RhiDevice,
        std::span<Rhi::ResourceView> RenderTargets,
        Rhi::ResourceView            DepthStencil)
    {
        auto& Nri     = RhiDevice.GetNRI();
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
        DeviceImpl&            RhiDevice,
        std::span<ClearDesc>   Clears,
        std::span<ClearRegion> Regions)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdClearAttachments(*m_CommandBuffer, Clears.data(), Count32(Clears), Regions.data(), Count32(Regions));
    }

    void CommandListImpl::SetViewports(
        DeviceImpl&         RhiDevice,
        std::span<Viewport> Viewports)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetViewports(*m_CommandBuffer, Viewports.data(), Count32(Viewports));
    }

    void CommandListImpl::SetScissorRects(
        DeviceImpl&            RhiDevice,
        std::span<ScissorRect> ScissorRects)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetScissors(*m_CommandBuffer, ScissorRects.data(), Count32(ScissorRects));
    }

    void CommandListImpl::SetStencilReference(
        DeviceImpl& RhiDevice,
        uint8_t     StencilReference)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetStencilReference(*m_CommandBuffer, StencilReference, StencilReference);
    }

    void CommandListImpl::SetDepthBounds(
        DeviceImpl& RhiDevice,
        float       MinDepthBounds,
        float       MaxDepthBounds)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetDepthBounds(*m_CommandBuffer, MinDepthBounds, MaxDepthBounds);
    }

    void CommandListImpl::SetBlendConstants(
        DeviceImpl&         RhiDevice,
        const Math::Color4& BlendConstants)
    {
        auto& Nri     = RhiDevice.GetNRI();
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
        DeviceImpl&                       RhiDeviceImpl,
        class Device&                     RhiDevice,
        std::span<const VertexBufferView> VertexBuffers,
        uint32_t                          BaseSlot)
    {
        std::vector<nri::Buffer*> NriBuffers;
        std::vector<uint64_t>     Offsets;

        NriBuffers.reserve(VertexBuffers.size());
        Offsets.reserve(VertexBuffers.size());

        for (const auto& VertexBuffer : VertexBuffers)
        {
            NriBuffers.push_back(VertexBuffer.Buffer.Unwrap(RhiDevice));
            Offsets.push_back(VertexBuffer.Offset);
        }

        auto& Nri     = RhiDeviceImpl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();
        NriCore.CmdSetVertexBuffers(*m_CommandBuffer, BaseSlot, Count32(NriBuffers), NriBuffers.data(), Offsets.data());
    }

    void CommandListImpl::SetIndexBuffer(
        DeviceImpl&            RhiDeviceImpl,
        class Device&          RhiDevice,
        const IndexBufferView& IndexBuffer)
    {
        auto& Nri     = RhiDeviceImpl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdSetIndexBuffer(*m_CommandBuffer, *IndexBuffer.Buffer.Unwrap(RhiDevice), IndexBuffer.Offset, IndexBuffer.Type);
    }

    void CommandListImpl::Draw(
        DeviceImpl&     RhiDevice,
        const DrawDesc& Desc)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDraw(*m_CommandBuffer, Desc);
    }

    void CommandListImpl::Draw(
        DeviceImpl&            RhiDevice,
        const DrawIndexedDesc& Desc)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdDrawIndexed(*m_CommandBuffer, Desc);
    }

    void CommandListImpl::EndRendering(
        DeviceImpl& RhiDevice)
    {
        auto& Nri     = RhiDevice.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CmdEndRendering(*m_CommandBuffer);
    }
} // namespace Ame::Rhi