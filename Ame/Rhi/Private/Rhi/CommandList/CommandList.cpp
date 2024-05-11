#include <Rhi/CommandList/CommandListImpl.hpp>
#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/Resource/VertexView.hpp>

namespace Ame::Rhi
{
    CommandList::CommandList(
        Device& RhiDevice) :
        m_Impl(RhiDevice.GetImpl().GetCurrentCommandList())
    {
    }

    void CommandList::BeginMarker(
        const char* Name)
    {
        m_Impl.get().BeginMarker(Name);
    }

    void CommandList::EndMarker()
    {
        m_Impl.get().EndMarker();
    }

    //

    void CommandList::SetPipelineLayout(
        const Ptr<PipelineLayout>& Layout)
    {
        m_Impl.get().SetPipelineLayout(Layout);
    }

    void CommandList::SetPipelineState(
        const Ptr<PipelineState>& Pipeline)
    {
        m_Impl.get().SetPipelineState(Pipeline);
    }

    //

    void CommandList::SetConstants(
        uint32_t    ConstantIndex,
        const void* Data,
        size_t      Size)
    {
        m_Impl.get().SetConstants(ConstantIndex, Data, Size);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets)
    {
        m_Impl.get().SetDescriptorSet(LayoutSlot, DescriptorSets, nullptr);
    }

    void CommandList::SetDescriptorSet(
        uint32_t             LayoutSlot,
        const DescriptorSet& DescriptorSets,
        uint32_t             DynamicBufferOffset)
    {
        m_Impl.get().SetDescriptorSet(LayoutSlot, DescriptorSets, &DynamicBufferOffset);
    }

    void CommandList::SetSamplePositions(
        std::span<SamplePosition> Positions,
        Sample_t                  SampleCount)
    {
        m_Impl.get().SetSamplePositions(Positions, SampleCount);
    }

    //

    DescriptorSet CommandList::AllocateSet(
        uint32_t LayoutSlot,
        uint32_t VariableCount)
    {
        return m_Impl.get().AllocateSet(LayoutSlot, VariableCount);
    }

    std::vector<DescriptorSet> CommandList::AllocateSets(
        uint32_t LayoutSlot,
        uint32_t InstanceCount,
        uint32_t VariableCount)
    {
        return m_Impl.get().AllocateSets(LayoutSlot, InstanceCount, VariableCount);
    }

    //

    void CommandList::BeginRendering(
        std::span<const Rhi::ResourceView*> RenderTargets,
        const Rhi::ResourceView*            DepthStencil)
    {
        m_Impl.get().BeginRendering(RenderTargets, DepthStencil);
    }

    void CommandList::ClearAttachments(
        std::span<ClearDesc>   Clears,
        std::span<ClearRegion> Regions)
    {
        m_Impl.get().ClearAttachments(Clears, Regions);
    }

    void CommandList::ClearAttachments(
        std::span<ClearDesc> Clears)
    {
        m_Impl.get().ClearAttachments(Clears);
    }

    void CommandList::SetViewports(
        std::span<Viewport> Viewports)
    {
        m_Impl.get().SetViewports(Viewports);
    }

    void CommandList::SetScissorRects(
        std::span<ScissorRect> ScissorRects)
    {
        m_Impl.get().SetScissorRects(ScissorRects);
    }

    void CommandList::SetStencilReference(
        uint8_t StencilReference)
    {
        m_Impl.get().SetStencilReference(StencilReference);
    }

    void CommandList::SetDepthBounds(
        float MinDepthBounds,
        float MaxDepthBounds)
    {
        m_Impl.get().SetDepthBounds(MinDepthBounds, MaxDepthBounds);
    }

    void CommandList::SetBlendConstants(
        const Math::Color4& BlendConstants)
    {
        m_Impl.get().SetBlendConstants(BlendConstants);
    }

    void CommandList::SetVertexBuffers(
        std::span<const VertexBufferView> VertexBuffers,
        uint32_t                          BaseSlot)
    {
        m_Impl.get().SetVertexBuffers(VertexBuffers, BaseSlot);
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
        m_Impl.get().SetIndexBuffer(IndexBuffer);
    }

    void CommandList::Draw(
        const DrawDesc& Desc)
    {
        m_Impl.get().Draw(Desc);
    }

    void CommandList::Draw(
        const DrawIndexedDesc& Desc)
    {
        m_Impl.get().Draw(Desc);
    }

    void CommandList::EndRendering()
    {
        m_Impl.get().EndRendering();
    }

    //

    void CommandList::Dispatch(
        uint32_t X,
        uint32_t Y,
        uint32_t Z)
    {
        m_Impl.get().Dispatch(X, Y, Z);
    }

    //

    void CommandList::CopyBuffer(
        const BufferCopyDesc& Src,
        const BufferCopyDesc& Dst,
        size_t                Size)
    {
        m_Impl.get().CopyBuffer(Src, Dst, Size);
    }

    void CommandList::CopyTexture(
        const TextureCopyDesc& Src,
        const TextureCopyDesc& Dst)
    {
        m_Impl.get().CopyTexture(Src, Dst);
    }

    void CommandList::UploadTexture(
        const TransferCopyDesc& Desc)
    {
        m_Impl.get().UploadTexture(Desc);
    }

    void CommandList::ReadbackTexture(
        const TransferCopyDesc& Desc)
    {
        m_Impl.get().ReadbackTexture(Desc);
    }

    //

    void CommandList::RequireState(
        const Buffer&      RhiBuffer,
        const AccessStage& State,
        bool               Append)
    {
        m_Impl.get().RequireState(RhiBuffer, State, Append);
    }

    void CommandList::RequireState(
        const Texture&            RhiTexture,
        const AccessLayoutStage&  State,
        const TextureSubresource& Subresource,
        bool                      Append)
    {
        m_Impl.get().RequireState(RhiTexture, State, Subresource, Append);
    }

    void CommandList::PlaceBarrier(
        const GlobalBarrierDesc& BarrierDesc)
    {
        m_Impl.get().PlaceBarrier(BarrierDesc);
    }

    void CommandList::CommitBarriers()
    {
        m_Impl.get().CommitBarriers();
    }

    //

    void CommandList::ClearBuffer(
        const ClearBufferDesc& Desc)
    {
        m_Impl.get().ClearBuffer(Desc);
    }

    void CommandList::ClearTexture(const ClearTextureDesc& Desc)
    {
        m_Impl.get().ClearTexture(Desc);
    }
} // namespace Ame::Rhi