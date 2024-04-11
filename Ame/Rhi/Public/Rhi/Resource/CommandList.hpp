#pragma once

#include <Rhi/Descs/CommandList.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
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
            uint32_t    ConstantIndex,
            const void* Data,
            size_t      Size);

        /// <summary>
        /// Set constants.
        /// </summary>
        template<typename Ty>
            requires std::is_standard_layout_v<Ty>
        void SetConstants(
            uint32_t  ConstantIndex,
            const Ty& Data)
        {
            SetConstants(ConstantIndex, &Data, sizeof(Ty));
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
        /// Set render targets.
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
        Device&          m_Device;
        CommandListImpl& m_Impl;
    };
} // namespace Ame::Rhi