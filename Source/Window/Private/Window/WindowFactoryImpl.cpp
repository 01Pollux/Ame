#include <Window/WindowFactoryImpl.hpp>
#include <Window/Glfw/WindowImplGlfw.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Window
{
    Ptr<IWindow> WindowFactoryImpl::CreateWindow(const WindowCreateDesc& createDesc)
    {
        Ptr<IWindow> window;
        switch (createDesc.Type)
        {
        case WindowType::DesktopWindow:
        {
            Ptr<WindowImplGlfw> windowGlfw;
            CreateInstance<WindowImplGlfw>(&windowGlfw);
            if (windowGlfw)
            {
                windowGlfw->Initialize(createDesc);
                windowGlfw->QueryInterface(IID_PTR(window));
            }
            break;
        }
        default:
        {
            Log::Window().Warning("Window type not supported");
            break;
        }
        }
        return window;
    }
} // namespace Ame::Window