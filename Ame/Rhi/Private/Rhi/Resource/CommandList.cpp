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
        const Ptr<PipelineLayout>& Layout)
    {
        m_Impl.SetPipelineLayout(Layout);
    }

    void CommandList::SetPipelineState(
        const Ptr<PipelineState>& Pipeline)
    {
        m_Impl.SetPipelineState(Pipeline);
    }

    //

    void CommandList::SetConstants(
        uint32_t    ConstantIndex,
        const void* Data,
        size_t      Size)
    {
        m_Impl.SetConstants(ConstantIndex, Data, Size);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets)
    {
        m_Impl.SetDescriptorSet(LayoutSlot, DescriptorSets, nullptr);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets,
        uint32_t             DynamicBufferOffset)
    {
        m_Impl.SetDescriptorSet(LayoutSlot, DescriptorSets, &DynamicBufferOffset);
    }

    void CommandList::SetSamplePositions(
        std::span<SamplePosition> Positions,
        Sample_t                  SampleCount)
    {
        m_Impl.SetSamplePositions(Positions, SampleCount);
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

    std::vector<DescriptorSet> CommandList::AllocateSets(
        uint32_t LayoutSlot,
        uint32_t InstanceCount,
        uint32_t VariableCount)
    {
        return m_Impl.AllocateSets(LayoutSlot, InstanceCount, VariableCount);
    }

    //

    void CommandList::BeginRendering(
        std::span<Rhi::ResourceView> RenderTargets,
        Rhi::ResourceView            DepthStencil)
    {
        m_Impl.BeginRendering(RenderTargets, DepthStencil);
    }

    void CommandList::ClearAttachments(
        std::span<ClearDesc>   Clears,
        std::span<ClearRegion> Regions)
    {
        m_Impl.ClearAttachments(Clears, Regions);
    }

    void CommandList::SetViewports(
        std::span<Viewport> Viewports)
    {
        m_Impl.SetViewports(Viewports);
    }

    void CommandList::SetScissorRects(
        std::span<ScissorRect> ScissorRects)
    {
        m_Impl.SetScissorRects(ScissorRects);
    }

    void CommandList::SetStencilReference(
        uint8_t StencilReference)
    {
        m_Impl.SetStencilReference(StencilReference);
    }

    void CommandList::SetDepthBounds(
        float MinDepthBounds,
        float MaxDepthBounds)
    {
        m_Impl.SetDepthBounds(MinDepthBounds, MaxDepthBounds);
    }

    void CommandList::SetBlendConstants(
        const Math::Color4& BlendConstants)
    {
        m_Impl.SetBlendConstants(BlendConstants);
    }

    void CommandList::SetVertexBuffers(
        std::span<const VertexBufferView> VertexBuffers,
        uint32_t                          BaseSlot)
    {
        m_Impl.SetVertexBuffers(VertexBuffers, BaseSlot);
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
        m_Impl.SetIndexBuffer(IndexBuffer);
    }

    void CommandList::Draw(
        const DrawDesc& Desc)
    {
        m_Impl.Draw(Desc);
    }

    void CommandList::Draw(
        const DrawIndexedDesc& Desc)
    {
        m_Impl.Draw(Desc);
    }

    void CommandList::EndRendering()
    {
        m_Impl.EndRendering();
    }
} // namespace Ame::Rhi