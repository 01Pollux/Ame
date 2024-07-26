#pragma once

#include <Module/Submodule.hpp>
#include <Window/Window.hpp>
#include <Module/Rhi/Config.hpp>

namespace Ame
{
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