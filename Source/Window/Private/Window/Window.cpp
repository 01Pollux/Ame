#include <Window/Glfw/WindowImplGlfw.hpp>

#include <Log/Logger.hpp>

namespace Ame::Window
{
    Ptr<IWindow> CreateWindow(const WindowType& windowType, const WindowCreateDesc& createDesc)
    {
        switch (windowType)
        {
        case WindowType::DesktopWindow:
        {
            return { ObjectAllocator<WindowImplGlfw>()(createDesc), IID_Window };
        }
        default:
        {
            AME_LOG_WARNING("Window type not supported");
            return {};
        }
        }
    }
} // namespace Ame::Window