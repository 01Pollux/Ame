#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    Buffer DeviceResourceAllocator::CreateBuffer(
        const BufferDesc& desc,
        MemoryLocation    location)
    {
        auto& deviceWrapper   = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& memoryAllocator = deviceWrapper.GetMemoryAllocator();

        auto buffer = memoryAllocator.CreateBuffer(location, desc);
        return Buffer(*this, buffer, location);
    }

    Texture DeviceResourceAllocator::CreateTexture(
        const TextureDesc& desc)
    {
        auto& deviceWrapper   = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& memoryAllocator = deviceWrapper.GetMemoryAllocator();

        auto texture = memoryAllocator.CreateTexture(desc);
        return Texture(*this, texture);
    }
} // namespace Ame::Rhi