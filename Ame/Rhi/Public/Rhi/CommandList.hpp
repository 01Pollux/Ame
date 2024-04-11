#pragma once

#include <Rhi/Descs/CommandList.hpp>

namespace Ame::Rhi
{
    class CommandList
    {
    public:
        CommandList(
            Device&             RhiDevice,
            nri::CommandBuffer& CommandBuffer);

    public:
        /// <summary>
        /// Set pipeline layout.
        /// </summary>
        void SetPipelineLayout(
            PipelineLayout& Layout);

        /// <summary>
        /// Set pipeline state.
        /// </summary>
        void SetPipelineState(
            PipelineState& Pipeline);

    public:
        /// <summary>
        /// Set constants.
        /// </summary>
        void SetConstants(
            const void* Data,
            size_t      Size,
            size_t      Offset = 0);

        /// <summary>
        /// Set constants.
        /// </summary>
        template<typename Ty>
            requires std::is_standard_layout_v<Ty>
        void SetConstants(
            const Ty& Data,
            uint32_t  Offset = 0)
        {
            SetConstants(&Data, sizeof(Ty), Offset);
        }

        /// <summary>
        /// Mandatory state, if enabled (can be set only once)
        /// Interacts with PSL enabled pipelines. Affects any depth-stencil operations, including clear and copy
        /// </summary>
        void SetSamplePositions(
            std::span<SamplePosition> Positions,
            Sample_t                  SampleCount);

    public:
        /// <summary>
        /// Set render targets and depth-stencil buffer.
        /// </summary>
        void BeginRendering(
            std::span<Rhi::ResourceView> RenderTargets,
            Rhi::ResourceView            DepthStencil = Rhi::ResourceView{});

        /// <summary>
        /// Clear render targets and depth-stencil buffer.
        /// </summary>
        void ClearAttachments(
            std::span<ClearDesc>   Clears,
            std::span<ClearRegion> Regions);

        /// <summary>
        /// Set viewport of the render target.
        /// </summary>
        void SetViewports(
            std::span<Viewport> Viewports);

        /// <summary>
        /// Set scissor rects of the render target.
        /// </summary>
        void SetScissorRects(
            std::span<ScissorRect> ScissorRects);

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
        /// End rendering.
        /// </summary>
        void EndRendering();

    private:
        Device&             m_Device;
        nri::CommandBuffer& m_CommandBuffer;
    };
} // namespace Ame::Rhi