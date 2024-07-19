#include <Rhi/Device/RhiDeviceImpl.hpp>

namespace Ame::Rhi
{
    Ptr<IRhiDevice> CreateRhiDevice(
        const DeviceCreateDesc& createDesc)
    {
        return { ObjectAllocator<RhiDeviceImpl>()(createDesc), IID_RhiDevice };
    }
} // namespace Ame::Rhi