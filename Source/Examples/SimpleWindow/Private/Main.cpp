#include <fstream>

#include <Window/WindowFactory.hpp>
#include <Window/DesktopWindow.hpp>

namespace Ame
{
    static void Main()
    {
        Pt::Initialize();

        Ptr<Window::IWindowFactory> windowFactory;
        windowFactory.create(Window::CLSID_WindowFactory);

        auto window = windowFactory->CreateWindow({ .Title = "Hello world" });
        windowFactory.reset();

        Ptr<Window::IDesktopWindow> desktopWindow;
        window.as(IID_PTR(desktopWindow));
        window.reset();

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