#include <Window/Glfw/DesktopWindowImplGlfw.hpp>

#include <Log/Logger.hpp>

namespace Ame::Window
{
    Ptr<IWindow> CreateWindow(const WindowType& windowType, const WindowCreateDesc& createDesc)
    {
        switch (windowType)
        {
        case WindowType::DesktopWindow:
        {
            return AmeCreate(DesktopWindowImplGlfw, createDesc);
        }
        default:
        {
            AME_LOG_WARNING("Window type not supported");
            return {};
        }
        }
    }
} // namespace Ame::Window