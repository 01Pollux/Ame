#include <Window/Window.hpp>

namespace Ame
{
    static void Main()
    {
        auto window = Window::CreateWindow(Window::WindowType::DesktopWindow, { .Title = "Hello world" });

        while (window->IsRunning())
        {
            window->ProcessEvents();
        }
    }
} // namespace Ame

int main()
{
    Ame::Main();
    return 0;
}