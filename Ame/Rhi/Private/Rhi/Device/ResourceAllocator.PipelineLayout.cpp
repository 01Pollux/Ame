#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device.Impl.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    Co::result<PipelineLayout> DeviceResourceAllocator::CreatePipelineLayout(
        const PipelineLayoutDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        nri::PipelineLayout* nriLayout = nullptr;
        ThrowIfFailed(nriCore.CreatePipelineLayout(
                          nriDevice, desc, nriLayout),
                      "Failed to create pipeline layout");

        co_return PipelineLayout(nriCore, nriLayout);
    }

    //

    Co::result<void> DeviceResourceAllocator::Release(
        PipelineLayout& layout)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriCore          = *nri.GetCoreInterface();

        nriCore.DestroyPipelineLayout(*layout.Unwrap());
        co_return;
    }
} // namespace Ame::Rhi