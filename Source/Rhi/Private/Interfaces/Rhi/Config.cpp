#include <Interfaces/Rhi/Config.hpp>
#include <Interfaces/Rhi/RhiDevice.hpp>
#include <Interfaces/Rhi/SurfaceWindow.hpp>
#include <Interfaces/Rhi/ImGuiRenderer.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

namespace Ame::Interfaces
{
    [[nodiscard]] static Ptr<Rhi::IImGuiRenderer> CreateImGuiRenderer(Rhi::IRhiDevice*                 rhiDevice,
                                                                      const GraphicsModuleImGuiConfig& imguiConfig,
                                                                      bool                             enabled)
    {
#ifndef AME_DIST
        if (enabled && imguiConfig.EnableImGuiSubmodule)
        {
            Rhi::ImGuiRendererCreateDesc imguiCreateDesc{ .RhiDevice = rhiDevice,
                                                          .InitialVertexBufferSize =
                                                              imguiConfig.InitialVertexBufferSize,
                                                          .InitialIndexBufferSize = imguiConfig.InitialIndexBufferSize,
                                                          .ConversionMode         = imguiConfig.ConversionMode,
                                                          .MultiThreaded          = imguiConfig.MultiThreaded };
            return Rhi::CreateImGuiRenderer(imguiCreateDesc);
        }
        else
#endif
        {
            return Rhi::CreateImGuiNullRenderer();
        }
    }

    void RhiModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        bool hasWindow = false;
        if (RhiDeviceDesc.Surface.has_value() && RhiDeviceDesc.Surface->Window)
        {
            registry->ExposeInterface(owner, IID_SurfaceWindow, RhiDeviceDesc.Surface->Window);
            hasWindow = true;
        }

        auto rhiDevice     = Rhi::CreateRhiDevice(RhiDeviceDesc);
        auto imguiRenderer = CreateImGuiRenderer(rhiDevice, ImGuiConfig, hasWindow);
        registry->ExposeInterface(owner, IID_RhiDevice, rhiDevice);
        registry->ExposeInterface(owner, IID_ImGuiRenderer, imguiRenderer);
    }
} // namespace Ame::Interfaces