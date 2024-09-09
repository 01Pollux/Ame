#include <fstream>

#include <Window/Window.hpp>
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