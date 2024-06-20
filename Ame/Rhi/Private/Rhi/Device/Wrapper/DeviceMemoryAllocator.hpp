#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Nri/Bridge.hpp>

namespace Ame::Rhi
{
    class DeviceCreateDesc;

    class IDeviceMemoryAllocator
    {
    protected:
        static constexpr uint64_t c_BufferSignature  = 0xDEAFBEEF;
        static constexpr uint64_t c_TextureSignature = 0xFEDBEEF;

    public:
        IDeviceMemoryAllocator() = default;

        IDeviceMemoryAllocator(const IDeviceMemoryAllocator&) = delete;
        IDeviceMemoryAllocator(IDeviceMemoryAllocator&&)      = delete;

        IDeviceMemoryAllocator& operator=(const IDeviceMemoryAllocator&) = delete;
        IDeviceMemoryAllocator& operator=(IDeviceMemoryAllocator&&)      = delete;

        virtual ~IDeviceMemoryAllocator() = default;

    public:
        [[nodiscard]] virtual nri::Buffer* CreateBuffer(
            MemoryLocation    memoryLocation,
            const BufferDesc& bufferDesc) = 0;

        virtual void ReleaseBuffer(
            nri::Buffer& buffer) = 0;

        [[nodiscard]] virtual nri::Texture* CreateTexture(
            const TextureDesc& textureDesc) = 0;

        virtual void ReleaseTexture(
            nri::Texture& texture) = 0;
    };
} // namespace Ame::Rhi