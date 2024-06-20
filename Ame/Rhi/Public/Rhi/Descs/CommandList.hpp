#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    using SamplePosition      = nri::SamplePosition;
    using AttachmentsDesc     = nri::AttachmentsDesc;
    using ClearRegion         = nri::Rect;
    using ClearBufferDesc     = nri::ClearStorageBufferDesc;
    using ClearTextureDesc    = nri::ClearStorageTextureDesc;
    using Viewport            = nri::Viewport;
    using ScissorRect         = nri::Rect;
    using DrawDesc            = nri::DrawDesc;
    using DrawIndexedDesc     = nri::DrawIndexedDesc;
    using DrawIndexedBaseDesc = nri::DrawIndexedBaseDesc;
    using DispatchDesc        = nri::DispatchDesc;

    using GlobalBarrierDesc  = nri::GlobalBarrierDesc;
    using BufferBarrierDesc  = nri::BufferBarrierDesc;
    using TextureBarrierDesc = nri::TextureBarrierDesc;
    using BarrierGroupDesc   = nri::BarrierGroupDesc;

    struct DrawIndirectDesc
    {
        nri::Buffer* DrawBuffer;
        uint64_t     DrawOffset;
        uint64_t     MaxDrawCount;

        nri::Buffer* CounterBuffer = nullptr;
        uint64_t     CounterOffset = 0;
    };

    //

    struct ClearDesc : nri::ClearDesc
    {
        using nri::ClearDesc::ClearDesc;

        ClearDesc() = default;
        ClearDesc(const nri::ClearDesc& desc) :
            nri::ClearDesc(desc)
        {
        }

        /// <summary>
        /// Shortcut for creating a clear render target descriptor.
        /// </summary>
        static ClearDesc RenderTarget(
            const Math::Color4& color,
            uint32_t            index = 0)
        {
            return { {
                .value{ .color32f{ color.r(), color.g(), color.b(), color.a() } },
                .attachmentContentType = nri::AttachmentContentType::COLOR,
                .colorAttachmentIndex  = index,
            } };
        }

        /// <summary>
        /// Shortcut for creating a clear depth-stencil descriptor.
        /// </summary>
        static ClearDesc DepthStencil(
            float   depth,
            uint8_t stencil)
        {
            return { {
                .value{ .depthStencil{ depth, stencil } },
                .attachmentContentType = nri::AttachmentContentType::DEPTH_STENCIL,
            } };
        }

        /// <summary>
        /// Shortcut for creating a clear depth descriptor.
        /// </summary>
        static ClearDesc Depth(
            float depth)
        {
            return { {
                .value{ .depthStencil{ depth, 0 } },
                .attachmentContentType = nri::AttachmentContentType::DEPTH,
            } };
        }

        /// <summary>
        /// Shortcut for creating a clear stencil descriptor.
        /// </summary>
        static ClearDesc Stencil(
            uint8_t stencil)
        {
            return { {
                .value{ .depthStencil{ 0, stencil } },
                .attachmentContentType = nri::AttachmentContentType::STENCIL,
            } };
        }
    };
} // namespace Ame::Rhi