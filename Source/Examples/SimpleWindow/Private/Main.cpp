#include <Window/Window.hpp>

namespace Ame
{
    static void Main()
    {
        auto window = Window::CreateWindow(Window::WindowType::DesktopWindow, { .Title = "Hello world" });

        while (window->IsRunning())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
} // namespace Ame

int main()
{
    Ame::Main();
    return 0;
}