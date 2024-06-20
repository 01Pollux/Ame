#include <Rhi/Device/CreateDesc.hpp>

#include <Rhi/Nri/Nri.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void DeviceCreateDesc::SetFirstAdapter()
    {
        uint32_t adapterCount = 1;
        ThrowIfFailed(nri::nriEnumerateAdapters(&Adapter, adapterCount), "Failed to enumerate adapters");
    }

    Co::generator<AdapterDesc> DeviceCreateDesc::EnumerateAdapters()
    {
        uint32_t adapterCount = 0;
        ThrowIfFailed(nri::nriEnumerateAdapters(nullptr, adapterCount), "Failed to enumerate adapters");
        std::vector<AdapterDesc> adapters(adapterCount);
        ThrowIfFailed(nri::nriEnumerateAdapters(adapters.data(), adapterCount), "Failed to enumerate adapters");

        for (auto& adapter : adapters)
        {
            co_yield adapter;
        }
    }
} // namespace Ame::Rhi