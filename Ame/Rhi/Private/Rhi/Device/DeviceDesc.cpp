#include <Rhi/DeviceDesc.hpp>
#include <NRI.h>
#include <Extensions/NRIDeviceCreation.h>

#include "../NriError.hpp"

namespace Ame::Rhi
{
    void DeviceCreateDesc::SetFirstAdapter()
    {
        uint32_t    AdapterCount = 1;
        ThrowIfFailed(nri::nriEnumerateAdapters(&Adapter, AdapterCount), "Failed to enumerate adapters");
    }

    Co::generator<AdapterDesc> DeviceCreateDesc::EnumerateAdapters()
    {
        uint32_t AdapterCount = 0;
        ThrowIfFailed(nri::nriEnumerateAdapters(nullptr, AdapterCount), "Failed to enumerate adapters");
        std::vector<AdapterDesc> Adapters(AdapterCount);

        for (auto& CurAdapter : Adapters)
        {
            co_yield CurAdapter;
        }
    }
} // namespace Ame::Rhi