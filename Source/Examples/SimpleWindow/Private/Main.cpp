#include <fstream>

#include <Window/WindowFactory.hpp>
#include <Window/DesktopWindow.hpp>

namespace Ame
{
    static void Main()
    {
        auto window = Window::CreateWindow(Window::WindowType::DesktopWindow, { .Title = "Hello world" });

        auto desktopWindow = window.Cast<Window::IDesktopWindow>(Window::IID_DesktopWindow);
        while (desktopWindow->IsRunning())
        {
            desktopWindow->ProcessEvents();
        }
    }
} // namespace Ame

int main()
{
    Ame::Main();
    return 0;
}