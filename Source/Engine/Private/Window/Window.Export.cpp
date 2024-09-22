#include <Window/Glfw/DesktopWindowImplGlfw.hpp>
#include <Window/Window.Export.h>

#include <Log/Logger.hpp>

// clang-format off
static_assert(sizeof(Ame_WindowCreateDesc_t) == sizeof(Ame::Window::WindowCreateDesc), "WindowCreateDesc size mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, Title) == offsetof(Ame::Window::WindowCreateDesc, Title), "WindowCreateDesc Title offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, Width) == offsetof(Ame::Window::WindowCreateDesc, Width), "WindowCreateDesc Width offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, Height) == offsetof(Ame::Window::WindowCreateDesc, Height), "WindowCreateDesc Height offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, CustomTitleBar) == offsetof(Ame::Window::WindowCreateDesc, CustomTitleBar), "WindowCreateDesc CustomTitleBar offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, StartInMiddle) == offsetof(Ame::Window::WindowCreateDesc, StartInMiddle), "WindowCreateDesc StartInMiddle offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, FullScreen) == offsetof(Ame::Window::WindowCreateDesc, FullScreen), "WindowCreateDesc FullScreen offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, Maximized) == offsetof(Ame::Window::WindowCreateDesc, Maximized), "WindowCreateDesc Maximized offset mismatch");
static_assert(offsetof(Ame_WindowCreateDesc_t, NoResize) == offsetof(Ame::Window::WindowCreateDesc, NoResize), "WindowCreateDesc NoResize offset mismatch");
// clang-format on

Ame_IWindow_t* Ame_IWindow_Create(Ame_WindowType_t windowType, const Ame_WindowCreateDesc_t* createDesc)
{
    switch (windowType)
    {
    case AME_WINDOW_TYPE_DESKTOP:
    {
        return std::bit_cast<Ame_IWindow_t*>(AmeCreateRaw(
            Ame::Window::DesktopWindowImplGlfw, *std::bit_cast<const Ame::Window::WindowCreateDesc*>(createDesc)));
    }
    default:
    {
        AME_LOG_WARNING("Window type not supported");
        return {};
    }
    }
}

bool Ame_IWindow_IsRunning(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->IsRunning();
}

void Ame_IWindow_Close(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->Close();
}

void Ame_IWindow_SetTitle(Ame_IWindow_t* window, Ame_StringView_t title)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->SetTitle({ title.Data, title.Size });
}

void Ame_IWindow_SetSize(Ame_IWindow_t* window, int width, int height)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->SetSize({ width, height });
}

void Ame_IWindow_SetPosition(Ame_IWindow_t* window, int x, int y)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->SetPosition({ x, y });
}

Ame_StringView_t Ame_IWindow_GetTitle(Ame_IWindow_t* window)
{
    auto  impl  = std::bit_cast<Ame::Window::IWindow*>(window);
    auto& title = impl->GetTitle();
    return { title.c_str(), title.size() };
}

void Ame_IWindow_GetSize(Ame_IWindow_t* window, int* width, int* height)
{
    auto        impl = std::bit_cast<Ame::Window::IWindow*>(window);
    const auto& size = impl->GetSize();
    *width           = size.Width();
    *height          = size.Height();
}

void Ame_IWindow_GetPosition(Ame_IWindow_t* window, int* x, int* y)
{
    auto        impl = std::bit_cast<Ame::Window::IWindow*>(window);
    const auto& pos  = impl->GetPosition();
    *x               = pos.x();
    *y               = pos.y();
}

bool Ame_IWindow_IsFullScreen(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->IsFullScreen();
}

void Ame_IWindow_SetFullscreen(Ame_IWindow_t* window, bool state)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->SetFullscreen(state);
}

bool Ame_IWindow_IsMinimized(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->IsMinimized();
}

bool Ame_IWindow_IsMaximized(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->IsMaximized();
}

void Ame_IWindow_Maximize(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->Maximize();
}

void Ame_IWindow_Minimize(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->Minimize();
}

bool Ame_IWindow_IsVisible(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->IsVisible();
}

void Ame_IWindow_SetVisible(Ame_IWindow_t* window, bool show)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->SetVisible(show);
}

bool Ame_IWindow_HasFocus(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    return impl->HasFocus();
}

void Ame_IWindow_RequestFocus(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->RequestFocus();
}

void Ame_IWindow_Restore(Ame_IWindow_t* window)
{
    auto impl = std::bit_cast<Ame::Window::IWindow*>(window);
    impl->Restore();
}
