#pragma once

#include <Rhi/Descs/CommandList.hpp>
#include <Rhi/CommandList/CopyDesc.hpp>

#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/Set.hpp>

namespace Ame::Rhi
{
    struct VertexBufferView;
    struct IndexBufferView;

    /// <summary>
    /// Command list is a sequence of commands that can be executed by the GPU.
    /// It is used to record rendering commands, dispatch compute shaders, and copy resources.
    ///
    /// They are not synchronized, and creating multiple command lists with the same device WILL reference the same device object.
    /// </summary>
    class CommandList
    {
    public:
        CommandList(
            Device& RhiDevice);

    public:
        /// <summary>
        /// Begin tag marker.
        /// </summary>
        void BeginMarker(
            const char* Name);

        /// <summary>
        /// End last tag marker.
        /// </summary>
        void EndMarker();

    public:
        /// <summary>
        /// Set pipeline layout.
        /// </summary>
        void SetPipelineLayout(
            const Ptr<PipelineLayout>& Layout);

        /// <summary>
        /// Set pipeline state.
        /// </summary>
        void SetPipelineState(
            const Ptr<PipelineState>& Pipeline);

    public:
        /// <summary>
        /// Set constants.
        /// </summary>
        void SetConstants(
            uint32_t    ConstantIndex,
            const void* Data,
            size_t      Size);

        /// <summary>
        /// Set constants.
        /// </summary>
        template<typename Ty>
            requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
        void SetConstants(
            uint32_t  ConstantIndex,
            const Ty& Data)
        {
            SetConstants(ConstantIndex, std::addressof(Data), sizeof(Ty));
        }

        /// <summary>
        /// Set descriptor sets with dynamic offsets if not null.
        /// </summary>
        void SetDescriptorSet(
            uint32_t             LayoutSlot,
            const DescriptorSet& DescriptorSets,
            const uint32_t*      DynamicBufferOffset = nullptr);

        /// <summary>
        /// Set descriptor sets with dynamic offsets if not null.
        /// </summary>
        void UnsetDescriptorSet(
            uint32_t LayoutSlot);

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<const SamplePosition> Positions,
            Sample_t                        SampleCount);

    public:
        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] DescriptorSet AllocateSet(
            uint32_t LayoutSlot,
            uint32_t VariableCount = 0);

        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
            uint32_t LayoutSlot,
            uint32_t InstanceCount = 1,
            uint32_t VariableCount = 0);

    public:
        /// <summary>
        /// Set render targets.
        /// </summary>
        void BeginRendering(
            std::span<const Rhi::ResourceView*> RenderTargets,
            const Rhi::ResourceView*            DepthStencil = nullptr);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<const ClearDesc>   Clears,
            std::span<const ClearRegion> Regions);

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<const ClearDesc> Clears);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewports(
            std::span<const Viewport> Viewports);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewport(
            const Viewport& Viewport);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            std::span<const ScissorRect> ScissorRects);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRect(
            const ScissorRect& ScissorRects);

        /// <summary>
        /// Set stencil reference value.
        /// </summary>
        void SetStencilReference(
            uint8_t StencilReference);

        /// <summary>
        /// Set depth bounds.
        /// </summary>
        void SetDepthBounds(
            float MinDepthBounds,
            float MaxDepthBounds);

        /// <summary>
        /// Set blend constants.
        /// </summary>
        void SetBlendConstants(
            const Math::Color4& BlendConstants);

        /// <summary>
        /// Set vertex buffers.
        /// </summary>
        void SetVertexBuffers(
            std::span<const VertexBufferView> VertexBuffers,
            uint32_t                          BaseSlot = 0);

        /// <summary>
        /// Set vertex buffer.
        /// </summary>
        void SetVertexBuffer(
            const VertexBufferView& VertexBuffer,
            uint32_t                BaseSlot = 0);

        /// <summary>
        /// Set index buffer.
        /// </summary>
        void SetIndexBuffer(
            const IndexBufferView& IndexBuffer);

        /// <summary>
        /// Perform draw call.
        /// </summary>
        void Draw(
            const DrawDesc& Desc);

        /// <summary>
        /// Perform draw indexed call.
        /// </summary>
        void Draw(
            const DrawIndexedDesc& Desc);

        /// <summary>
        /// Perform an indirect draw call.
        /// </summary>
        void DrawIndirect(
            const DrawIndirectDesc& Desc);

        /// <summary>
        /// Perform an indirect indexed draw call.
        /// </summary>
        void DrawIndirectIndexed(
            const DrawIndirectDesc& Desc);

        /// <summary>
        /// End rendering.
        /// </summary>
        void EndRendering();

    public:
        /// <summary>
        /// Dispatch compute operation
        /// </summary>
        void Dispatch(
            uint32_t X,
            uint32_t Y = 1,
            uint32_t Z = 1);

    public:
        /// <summary>
        /// Perform a copy from buffer to buffer
        /// Copy the rest of buffer if size is 0
        /// </summary>
        void CopyBuffer(
            const BufferCopyDesc& Src,
            const BufferCopyDesc& Dst,
            size_t                Size = 0);

        /// <summary>
        /// Perform a copy from texture to texture
        /// </summary>
        void CopyTexture(
            const TextureCopyDesc& Src,
            const TextureCopyDesc& Dst);

        /// <summary>
        /// Perform a copy from buffer to texture
        /// </summary>
        void UploadTexture(
            const TransferCopyDesc& Desc);

        /// <summary>
        /// Perform a copy from texture to buffer
        /// </summary>
        void ReadbackTexture(
            const TransferCopyDesc& Desc);

    public:
        /// <summary>
        /// Require a buffer to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Buffer&      RhiBuffer,
            const AccessStage& State,
            bool               Append = false);

        /// <summary>
        /// Require a texture to be in a certain state
        /// if Append is true, the state will be appended to the current state
        /// </summary>
        void RequireState(
            const Texture&            RhiTexture,
            const AccessLayoutStage&  State,
            const TextureSubresource& Subresource = AllSubresources,
            bool                      Append      = false);

        /// <summary>
        /// Place a global barrier
        /// </summary>
        void PlaceBarrier(
            const GlobalBarrierDesc& BarrierDesc);

        /// <summary>
        /// Commit all the pending barriers
        /// </summary>
        void CommitBarriers();

    public:
        /// <summary>
        /// Clear storage buffer
        /// </summary>
        void ClearBuffer(
            const ClearBufferDesc& Desc);

        /// <summary>
        /// Clear storage texture
        /// </summary>
        void ClearTexture(
            const ClearTextureDesc& Desc);

    private:
        Ref<CommandListImpl> m_Impl;
    };
} // namespace Ame::Rhi