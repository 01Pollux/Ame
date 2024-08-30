#pragma once

#include <Module/Submodule.hpp>
#include <Module/Core/Config.hpp>
#include <Plugin/PluginHost.hpp>

namespace Ame
{
    class PluginHostSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_PluginHostSubmodule, IID_BaseSubmodule, Base);

    public:
        explicit PluginHostSubmodule(
            IReferenceCounters* counters);

    public:
        [[nodiscard]] IPluginHost* GetPluginHost() const noexcept
        {
            return m_PluginHost.get();
        }

    private:
        UniquePtr<IPluginHost> m_PluginHost;
    };
} // namespace Ame