#pragma once

#include <Module/Submodule.hpp>
#include <Window/Window.hpp>

namespace Ame
{
    // {640F1A0F-8EBC-4722-931D-92EAD1EFFE74}
    static const UId IID_PlatformWindowSubmodule = { 0x640f1a0f, 0x8ebc, 0x4722, { 0x93, 0x1d, 0x92, 0xea, 0xd1, 0xef, 0xfe, 0x74 } };

    class PlatformWindowSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_PlatformWindowSubmodule, IID_BaseSubmodule, Base,
            m_Window);

    public:
        PlatformWindowSubmodule(
            IReferenceCounters*  counters,
            Ptr<Window::IWindow> window) :
            Base(counters, IID_PlatformWindowSubmodule),
            m_Window(std::move(window))
        {
        }

    private:
        Ptr<Window::IWindow> m_Window;
    };
} // namespace Ame