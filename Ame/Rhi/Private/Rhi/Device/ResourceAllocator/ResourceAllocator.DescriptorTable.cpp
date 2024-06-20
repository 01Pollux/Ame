#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/Resource/DescriptorTable.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    DescriptorTable DeviceResourceAllocator::CreateDescriptorTable(
        const DescriptorPoolDesc& desc)
    {
        auto& deviceWrapper = GetWrapper();
        auto& nri           = deviceWrapper.GetNri();
        auto& nriDevice     = deviceWrapper.GetNriDevice();
        auto& nriCore       = *nri.GetCoreInterface();

        nri::DescriptorPool* descriptorPool = nullptr;
        ThrowIfFailed(
            nriCore.CreateDescriptorPool(nriDevice, desc, descriptorPool),
            "Failed to create descriptor pool");

        return DescriptorTable(*this, descriptorPool);
    }
} // namespace Ame::Rhi