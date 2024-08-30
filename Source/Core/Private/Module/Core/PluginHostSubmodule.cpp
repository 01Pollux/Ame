#include <Module/Core/PluginHostSubmodule.hpp>
#include <Plugin/PluginHostImpl.hpp>

namespace Ame
{
    PluginHostSubmodule::PluginHostSubmodule(IReferenceCounters* counters):
        Base(counters, IID_PluginHostSubmodule),
        m_PluginHost(std::make_unique<PluginHostImpl>())
    {
    }
} // namespace Ame