#pragma once

#include <Module/Module.hpp>

#include <Module/Core/TimeSubmodule.hpp>
#include <Module/Core/FrameEventSubmodule.hpp>
#include <Module/Core/CoroutineSubmodule.hpp>
#include <Module/Core/StateSubmodule.hpp>

namespace Ame
{
    // {C9E70A4E-1AA4-4B93-B086-05314A078048}
    static const UId IID_CoreModule = { 0xc9e70a4e, 0x1aa4, 0x4b93, { 0xb0, 0x86, 0x5, 0x31, 0x4a, 0x7, 0x80, 0x48 } };

    class CoreModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_CoreModule, IID_BaseModule, Base,
            m_TimeSubmodule,
            m_FrameEventSubmodule,
            m_CoroutineSubmodule,
            m_StateSubmodule)

    public:
        CoreModule(
            IReferenceCounters* counters) :
            Base(counters, IID_CoreModule),
            m_TimeSubmodule(ObjectAllocator<TimeSubmodule>(this)()),
            m_FrameEventSubmodule(ObjectAllocator<FrameEventSubmodule>(this)()),
            m_CoroutineSubmodule(ObjectAllocator<CoroutineSubmodule>(this)()),
            m_StateSubmodule(ObjectAllocator<StateSubmodule>(this)())
        {
        }

    private:
        Ptr<TimeSubmodule>       m_TimeSubmodule;
        Ptr<FrameEventSubmodule> m_FrameEventSubmodule;
        Ptr<CoroutineSubmodule>  m_CoroutineSubmodule;
        Ptr<StateSubmodule>      m_StateSubmodule;
    }; // namespace Ame
} // namespace Ame