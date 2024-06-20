#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device.Impl.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    Co::result<Buffer> DeviceResourceAllocator::CreateBuffer(
        const BufferDesc& desc,
        MemoryLocation    location)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& memoryAllocator  = rhiDeviceWrapper.GetMemoryAllocator();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriCore          = *nri.GetCoreInterface();

        auto buffer = memoryAllocator.CreateBuffer(location, desc);
        co_return Buffer(nriCore, buffer, location);
    }

    Co::result<Texture> DeviceResourceAllocator::CreateTexture(
        const TextureDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& memoryAllocator  = rhiDeviceWrapper.GetMemoryAllocator();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriCore          = *nri.GetCoreInterface();

        auto texture = memoryAllocator.CreateTexture(desc);
        co_return Texture(nriCore, texture);
    }

    //

    Co::result<void> DeviceResourceAllocator::Release(
        Buffer& buffer)
    {
        auto& frameManager = m_RhiDeviceImpl.get().GetFrameManager();
        auto& frame        = frameManager.GetCurrentFrame();

        auto lock = co_await TryLock();
        frame.DeferRelease(*buffer.Unwrap());
    }

    Co::result<void> DeviceResourceAllocator::Release(
        Texture& texture)
    {
        auto& frameManager = m_RhiDeviceImpl.get().GetFrameManager();
        auto& frame        = frameManager.GetCurrentFrame();

        auto lock = co_await TryLock();
        frame.DeferRelease(*texture.Unwrap());
    }
} // namespace Ame::Rhi