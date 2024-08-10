#include <Rhi/Device/RhiDeviceImpl.hpp>

namespace Ame::Rhi
{
    Ptr<IRhiDevice> CreateRhiDevice(
        const DeviceCreateDesc& createDesc)
    {
        return Ptr(ObjectAllocator<RhiDeviceImpl>()(createDesc));
    }
} // namespace Ame::Rhi