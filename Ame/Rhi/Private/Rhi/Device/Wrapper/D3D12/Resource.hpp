#pragma once

#ifdef AME_PLATFORM_WINDOWS
struct AGSContext;

#include <Rhi/Device/Wrapper/D3D12/Core.hpp>
#include <Rhi/Device/Wrapper/DeviceMemoryAllocator.hpp>

#include <D3D12MemAlloc.h>

#include <Rhi/Device/MemoryDesc.hpp>

namespace Ame::Rhi::D3D12
{
    struct DxgiFormat
    {
        DXGI_FORMAT Typeless;
        DXGI_FORMAT Typed;

        [[nodiscard]] static constexpr DxgiFormat Get(
            ResourceFormat format);
    };

    //

    [[nodiscard]] D3D12_RESOURCE_DESC GetResourceDesc(
        const BufferDesc& bufferDesc);

    [[nodiscard]] D3D12_RESOURCE_DESC GetResourceDesc(
        const TextureDesc& textureDesc);
} // namespace Ame::Rhi::D3D12
#endif
