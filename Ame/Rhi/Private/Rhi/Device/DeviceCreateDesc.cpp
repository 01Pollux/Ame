#include <Rhi/DeviceCreateDesc.hpp>

#include "../Nri/Nri.hpp"
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
        ThrowIfFailed(nri::nriEnumerateAdapters(Adapters.data(), AdapterCount), "Failed to enumerate adapters");

        for (auto& CurAdapter : Adapters)
        {
            co_yield CurAdapter;
        }
    }
} // namespace Ame::Rhi