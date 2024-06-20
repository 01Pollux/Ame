#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    PipelineLayout DeviceResourceAllocator::CreatePipelineLayout(
        const PipelineLayoutDesc& desc)
    {
        auto& deviceWrapper = GetWrapper();
        auto& nri           = deviceWrapper.GetNri();
        auto& nriDevice     = deviceWrapper.GetNriDevice();
        auto& nriCore       = *nri.GetCoreInterface();

        nri::PipelineLayout* nriLayout = nullptr;
        ThrowIfFailed(
            nriCore.CreatePipelineLayout(nriDevice, desc, nriLayout),
            "Failed to create pipeline layout");

        return PipelineLayout(*this, nriLayout);
    }
} // namespace Ame::Rhi