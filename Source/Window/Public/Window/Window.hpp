#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>

#include <Window/WindowCreateDesc.hpp>

namespace Ame::Window
{
    // {D04401D7-E3E3-45D8-B66D-FA5E05750D90}
    inline constexpr UId IID_Window{ 0xd04401d7, 0xe3e3, 0x45d8, { 0xb6, 0x6d, 0xfa, 0x5e, 0x5, 0x75, 0xd, 0x90 } };

    class WindowEventListener;

    class IWindow : public IObject
    {
    public:
        virtual void               ProcessEvents()   = 0;
        [[nodiscard]] virtual bool IsRunning() const = 0;
        virtual void               Close()           = 0;

        virtual void SetTitle(const String& title)               = 0;
        virtual void SetSize(const Math::Size2I& size)           = 0;
        virtual void SetPosition(const Math::Vector2I& position) = 0;

        [[nodiscard]] virtual const String&       GetTitle() const    = 0;
        [[nodiscard]] virtual const Math::Size2I& GetSize() const     = 0;
        [[nodiscard]] virtual Math::Vector2I      GetPosition() const = 0;

        [[nodiscard]] virtual WindowEventListener& GetEventListener() = 0;
    };

    [[nodiscard]] Ptr<IWindow> CreateWindow(const WindowType& windowType, const WindowCreateDesc& createDesc);
} // namespace Ame::Window
