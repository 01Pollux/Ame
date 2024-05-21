#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    using SamplePosition      = nri::SamplePosition;
    using AttachmentsDesc     = nri::AttachmentsDesc;
    using ClearDesc           = nri::ClearDesc;
    using ClearRegion         = nri::Rect;
    using ClearBufferDesc     = nri::ClearStorageBufferDesc;
    using ClearTextureDesc    = nri::ClearStorageTextureDesc;
    using Viewport            = nri::Viewport;
    using ScissorRect         = nri::Rect;
    using DrawDesc            = nri::DrawDesc;
    using DrawIndexedDesc     = nri::DrawIndexedDesc;
    using DrawIndexedBaseDesc = nri::DrawIndexedBaseDesc;
    using DispatchDesc        = nri::DispatchDesc;

    struct DrawIndirectDesc
    {
        nri::Buffer* DrawBuffer;
        uint64_t     DrawOffset;
        uint64_t     MaxDrawCount;

        nri::Buffer* CounterBuffer = nullptr;
        uint64_t     CounterOffset = 0;
    };
} // namespace Ame::Rhi