#pragma once

#include <Window/Window.hpp>
#include <Interfaces/Rhi/Config.hpp>

namespace Ame::Interfaces
{
    class WindowBackend final : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(IID_WindowBackend, Base, m_Window);

    private:
        IMPLEMENT_INTERFACE_CTOR(WindowBackend, Ptr<Window::IWindow> window) :
            Base(counters), m_Window(std::move(window))
        {
        }

    public:
        [[nodiscard]] Window::IWindow* GetWindow() const noexcept
        {
            return m_Window;
        }

    private:
        Ptr<Window::IWindow> m_Window;
    };
} // namespace Ame::Interfaces