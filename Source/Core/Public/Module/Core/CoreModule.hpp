#pragma once

#include <Module/Module.hpp>

#include <Module/Core/Config.hpp>
#include <Module/Core/TimeSubmodule.hpp>
#include <Module/Core/FrameEventSubmodule.hpp>

namespace Ame
{
    class CoreModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_CoreModule, IID_BaseModule, Base,
            m_TimeSubmodule,
            m_FrameEventSubmodule);

    public:
        CoreModule(
            IReferenceCounters* counters,
            CoreModuleConfig    config) :
            Base(counters, IID_CoreModule),
            m_TimeSubmodule(config.EnableTimeSubmodule ? ObjectAllocator<TimeSubmodule>(this)() : nullptr),
            m_FrameEventSubmodule(config.EnableFrameEventSubmodule ? ObjectAllocator<FrameEventSubmodule>(this)() : nullptr)
        {
        }

    private:
        Ptr<TimeSubmodule>       m_TimeSubmodule;
        Ptr<FrameEventSubmodule> m_FrameEventSubmodule;
    }; // namespace Ame
} // namespace Ame