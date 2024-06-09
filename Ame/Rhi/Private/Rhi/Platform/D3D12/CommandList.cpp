#include <Rhi/Platform/D3D12/CommandList.hpp>

#ifdef AME_PLATFORM_WINDOWS
#include <d3d12sdklayers.h>

namespace Ame::Rhi::D3D12
{
#ifndef AME_DIST
    constexpr uint32_t               PIXEvent_MarkerMetadata = 2;
    std::pair<const void*, uint32_t> Encode_Pix3Blob(
        const char*  label,
        uint32_t     size,
        Math::Color4 color)
    {
        color *= 255.0f;

        thread_local uint64_t blob[64];

        constexpr uint64_t PIXEvent_BeginEvent_NoArgs_Mask = 0x2ull << 10;
        constexpr uint64_t PIXEvent_CopyChunkSize_Mask     = 8ull << 55;
        constexpr uint64_t PIXEvent_IsANSI_Mask            = 1ull << 54;

        blob[0] = PIXEvent_BeginEvent_NoArgs_Mask;
        blob[1] = ColorToU32(color);
        blob[2] = PIXEvent_CopyChunkSize_Mask | PIXEvent_IsANSI_Mask;

        // fill the rest with zeros
        std::fill_n(blob + 3, 61, 0);

        // copy the label
        size = std::min(size, uint32_t(sizeof(blob) - (4 * sizeof(UINT64))));
        std::copy_n(label, size, std::bit_cast<char*>(blob + 3));

        return { blob, static_cast<uint32_t>(sizeof(blob)) };
    }
#endif

    void CommandList_BeginMarker(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList,
        const String&       label,
        const Math::Color4& color)
    {
#ifndef AME_DIST
        auto nativeObject     = coreInterface.GetCommandBufferNativeObject(commandList);
        auto d3d12CommandList = std::bit_cast<ID3D12GraphicsCommandList*>(nativeObject);

        auto [blob, size] = Encode_Pix3Blob(label.c_str(), label.size(), color);
        d3d12CommandList->BeginEvent(PIXEvent_MarkerMetadata, blob, size);
#endif
    }

    void CommandList_MarkEvent(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList,
        const String&       label,
        const Math::Color4& color)
    {
#ifndef AME_DIST
        auto nativeObject     = coreInterface.GetCommandBufferNativeObject(commandList);
        auto d3d12CommandList = std::bit_cast<ID3D12GraphicsCommandList*>(nativeObject);

        auto [blob, size] = Encode_Pix3Blob(label.c_str(), label.size(), color);
        d3d12CommandList->SetMarker(PIXEvent_MarkerMetadata, blob, size);
#endif
    }

    void CommandList_EndMarker(
        nri::CoreInterface& coreInterface,
        nri::CommandBuffer& commandList)
    {
#ifndef AME_DIST
        auto nativeObject     = coreInterface.GetCommandBufferNativeObject(commandList);
        auto d3d12CommandList = std::bit_cast<ID3D12GraphicsCommandList*>(nativeObject);

        d3d12CommandList->EndEvent();
#endif
    }
} // namespace Ame::Rhi::D3D12
#endif