#include <fstream>

#include <Window/DesktopWindow.hpp>
#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    static auto GetDeviceDesc(Ptr<Window::IWindow> window)
    {
        return Rhi::DeviceCreateDesc{ .Types{ Rhi::DeviceCreateDescD3D12{}, Rhi::DeviceCreateDescVulkan{},
                                              Rhi::DeviceCreateDescD3D11{}, Rhi::DeviceCreateDescGL{} },
                                      .Surface = Rhi::RenderSurfaceDesc{ std::move(window) } };
    }

    static void Main()
    {
        auto window    = Window::CreateWindow(Window::WindowType::DesktopWindow, { .Title = "Hello world" });
        auto rhiDevice = Rhi::CreateRhiDevice(GetDeviceDesc(window));

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