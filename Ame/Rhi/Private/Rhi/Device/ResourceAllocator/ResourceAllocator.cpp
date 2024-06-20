#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

namespace Ame::Rhi
{
    DeviceResourceAllocator::DeviceResourceAllocator(
        DeviceImpl& rhiDeviceImpl,
        bool        MultiThreaded) :
        m_RhiDeviceImpl(rhiDeviceImpl),
        m_MultiThreaded(MultiThreaded)
    {
    }

    const Rhi::DeviceDesc& DeviceResourceAllocator::GetDeviceDesc() const noexcept
    {
        auto& deviceWrapper = GetWrapper();
        auto& nri           = deviceWrapper.GetNri();
        auto& nriDevice     = deviceWrapper.GetNriDevice();
        auto& nriCore       = *nri.GetCoreInterface();
        return nriCore.GetDeviceDesc(nriDevice);
    }

    nri::CoreInterface& DeviceResourceAllocator::GetNriCore() const noexcept
    {
        auto& deviceWrapper = GetWrapper();
        return *deviceWrapper.GetNri().GetCoreInterface();
    }

    FrameManager& DeviceResourceAllocator::GetFrameManager() const noexcept
    {
        return m_RhiDeviceImpl.get().GetFrameManager();
    }

    IDeviceWrapper& DeviceResourceAllocator::GetWrapper() const noexcept
    {
        return m_RhiDeviceImpl.get().GetDeviceWrapper();
    }
} // namespace Ame::Rhi