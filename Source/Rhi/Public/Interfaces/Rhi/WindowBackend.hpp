#pragma once

#include <Core/Interface.hpp>
#include <Window/Window.hpp>

namespace Ame::Interfaces
{
    // {640F1A0F-8EBC-4722-931D-92EAD1EFFE74}
    inline constexpr UId IID_WindowBackend{
        0x640f1a0f, 0x8ebc, 0x4722, { 0x93, 0x1d, 0x92, 0xea, 0xd1, 0xef, 0xfe, 0x74 }
    };

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