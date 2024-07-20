#pragma once

#include <Module/Module.hpp>

#include <Module/Rhi/Config.hpp>
#include <Module/Rhi/PlatformWindowSubmodule.hpp>
#include <Module/Rhi/RhiGraphicsSubmodule.hpp>
#include <Module/Rhi/ImGuiSubmodule.hpp>

namespace Ame
{
    class RhiModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_RhiModule, IID_BaseModule, Base,
            m_PlatformWindowSubmodule,
            m_RhiGraphicsSubmodule,
            m_ImGuiSubmodule);

    public:
        RhiModule(
            IReferenceCounters*    counters,
            const RhiModuleConfig& config);

    private:
        Ptr<PlatformWindowSubmodule> m_PlatformWindowSubmodule;
        Ptr<RhiGraphicsSubmodule>    m_RhiGraphicsSubmodule;
        Ptr<ImGuiSubmodule>          m_ImGuiSubmodule;
    }; // namespace Ame
} // namespace Ame