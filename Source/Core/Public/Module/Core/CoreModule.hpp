#pragma once

#include <Module/Module.hpp>

#include <Module/Core/Config.hpp>
#include <Module/Core/TimeSubmodule.hpp>
#include <Module/Core/FrameEventSubmodule.hpp>
#include <Module/Core/PluginHostSubmodule.hpp>

namespace Ame
{
    class CoreModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_CoreModule, IID_BaseModule, Base,
            m_TimeSubmodule,
            m_FrameEventSubmodule,
            m_PluginHostSubmodule);

    public:
        CoreModule(
            IReferenceCounters* counters,
            CoreModuleConfig    config) :
            Base(counters, IID_CoreModule),
            m_TimeSubmodule(config.EnableTimeSubmodule ? ObjectAllocator<TimeSubmodule>(this)() : nullptr),
            m_FrameEventSubmodule(config.EnableFrameEventSubmodule ? ObjectAllocator<FrameEventSubmodule>(this)() : nullptr),
            m_PluginHostSubmodule(config.EnablePluginHostSubmodule ? ObjectAllocator<PluginHostSubmodule>(this)() : nullptr)
        {
        }

    private:
        Ptr<TimeSubmodule>       m_TimeSubmodule;
        Ptr<FrameEventSubmodule> m_FrameEventSubmodule;
        Ptr<PluginHostSubmodule> m_PluginHostSubmodule;
    }; // namespace Ame
} // namespace Ame