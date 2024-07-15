#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>
#include <Math/Size2.hpp>

namespace Ame::Window
{
    AME_INTERFACE_BEGIN("{d4c5a174-763c-4e6b-83f8-73355002fb7f}", IWindow);

    virtual void               ProcessEvents()   = 0;
    [[nodiscard]] virtual bool IsRunning() const = 0;
    virtual void               Close()           = 0;

    virtual void SetTitle(const String& title)               = 0;
    virtual void SetSize(const Math::Size2I& size)           = 0;
    virtual void SetPosition(const Math::Vector2I& position) = 0;

    [[nodiscard]] virtual const String&       GetTitle() const    = 0;
    [[nodiscard]] virtual const Math::Size2I& GetSize() const     = 0;
    [[nodiscard]] virtual Math::Vector2I      GetPosition() const = 0;

    AME_INTERFACE_END();
} // namespace Ame::Window
