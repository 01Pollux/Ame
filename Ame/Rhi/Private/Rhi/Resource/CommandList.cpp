#include <Rhi/Resource/CommandListImpl.hpp>
#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/Resource/VertexView.hpp>

namespace Ame::Rhi
{
    CommandList::CommandList(
        Device& RhiDevice) :
        m_Device(RhiDevice),
        m_Impl(m_Device.GetImpl().GetCurrentCommandList())
    {
    }

    void CommandList::SetPipelineLayout(
        PipelineLayout& Layout)
    {
        m_Impl.SetPipelineLayout(m_Device.GetImpl(), Layout);
    }

    void CommandList::SetPipelineState(
        PipelineState& Pipeline)
    {
        m_Impl.SetPipelineState(m_Device.GetImpl(), Pipeline);
    }

    //

    void CommandList::SetConstants(
        uint32_t    ConstantIndex,
        const void* Data,
        size_t      Size)
    {
        m_Impl.SetConstants(m_Device.GetImpl(), ConstantIndex, Data, Size);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets)
    {
		m_Impl.SetDescriptorSet(m_Device.GetImpl(), LayoutSlot, DescriptorSets, nullptr);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets,
        uint32_t             DynamicBufferOffset)
    {
		m_Impl.SetDescriptorSet(m_Device.GetImpl(), LayoutSlot, DescriptorSets, &DynamicBufferOffset);
    }

    void CommandList::SetSamplePositions(
        std::span<SamplePosition> Positions,
        Sample_t                  SampleCount)
    {
        m_Impl.SetSamplePositions(m_Device.GetImpl(), Positions, SampleCount);
    }

    //

    Streaming::BufferOStream CommandList::AllocateUpload(
        size_t Size,
        size_t WindowSize)
    {
        return Streaming::BufferOStream();
    }

    Streaming::BufferOStream CommandList::AllocateScratch(
        size_t Size,
        size_t WindowSize)
    {
        return Streaming::BufferOStream();
    }

    std::vector<DescriptorSet*> CommandList::AllocateSets(
        uint32_t Count)
    {
        return m_Impl.AllocateSets(m_Device.GetImpl(), Count);
    }

    //

    void CommandList::BeginRendering(
        std::span<Rhi::ResourceView> RenderTargets,
        Rhi::ResourceView            DepthStencil)
    {
        m_Impl.BeginRendering(m_Device.GetImpl(), RenderTargets, DepthStencil);
    }

    void CommandList::ClearAttachments(
        std::span<ClearDesc>   Clears,
        std::span<ClearRegion> Regions)
    {
        m_Impl.ClearAttachments(m_Device.GetImpl(), Clears, Regions);
    }

    void CommandList::SetViewports(
        std::span<Viewport> Viewports)
    {
        m_Impl.SetViewports(m_Device.GetImpl(), Viewports);
    }

    void CommandList::SetScissorRects(
        std::span<ScissorRect> ScissorRects)
    {
        m_Impl.SetScissorRects(m_Device.GetImpl(), ScissorRects);
    }

    void CommandList::SetStencilReference(
        uint8_t StencilReference)
    {
        m_Impl.SetStencilReference(m_Device.GetImpl(), StencilReference);
    }

    void CommandList::SetDepthBounds(
        float MinDepthBounds,
        float MaxDepthBounds)
    {
        m_Impl.SetDepthBounds(m_Device.GetImpl(), MinDepthBounds, MaxDepthBounds);
    }

    void CommandList::SetBlendConstants(
        const Math::Color4& BlendConstants)
    {
        m_Impl.SetBlendConstants(m_Device.GetImpl(), BlendConstants);
    }

    void CommandList::SetVertexBuffers(
        std::span<const VertexBufferView> VertexBuffers,
        uint32_t                          BaseSlot)
    {
        m_Impl.SetVertexBuffers(m_Device.GetImpl(), m_Device, VertexBuffers, BaseSlot);
    }

    void CommandList::SetVertexBuffer(
        const VertexBufferView& VertexBuffer,
        uint32_t                BaseSlot)
    {
        SetVertexBuffers({ &VertexBuffer, 1 }, BaseSlot);
    }

    void CommandList::SetIndexBuffer(
        const IndexBufferView& IndexBuffer)
    {
        m_Impl.SetIndexBuffer(m_Device.GetImpl(), m_Device, IndexBuffer);
    }

    void CommandList::Draw(
        const DrawDesc& Desc)
    {
        m_Impl.Draw(m_Device.GetImpl(), Desc);
    }

    void CommandList::Draw(
        const DrawIndexedDesc& Desc)
    {
        m_Impl.Draw(m_Device.GetImpl(), Desc);
    }

    void CommandList::EndRendering()
    {
        m_Impl.EndRendering(m_Device.GetImpl());
    }
} // namespace Ame::Rhi