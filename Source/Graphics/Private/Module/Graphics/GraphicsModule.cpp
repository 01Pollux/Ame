#include <Module/Graphics/GraphicsModule.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

namespace Ame
{
    [[nodiscard]] static PlatformWindowSubmodule* CreateWindowSubmodule(
        const Rhi::DeviceCreateDesc& desc)
    {
        return (desc.Surface.has_value() && desc.Surface->Window) ? ObjectAllocator<PlatformWindowSubmodule>()(desc.Surface->Window) : nullptr;
    }

    [[nodiscard]] static Ptr<Rhi::IImGuiRenderer> CreateImGuiRenderer(
        RhiGraphicsSubmodule*            graphicsSubmodule,
        const GraphicsModuleImGuiConfig& imguiConfig,
        bool                             enabled)
    {
#ifndef AME_DIST
        if (enabled && imguiConfig.EnableImGuiSubmodule)
        {
            Ptr<Rhi::IRhiDevice> rhiDevice;
            graphicsSubmodule->QueryInterface(Rhi::IID_RhiDevice, rhiDevice.DblPtr<IObject>());

            Rhi::ImGuiRendererCreateDesc imguiCreateDesc{
                .RhiDevice               = rhiDevice,
                .InitialVertexBufferSize = imguiConfig.InitialVertexBufferSize,
                .InitialIndexBufferSize  = imguiConfig.InitialIndexBufferSize,
                .ConversionMode          = imguiConfig.ConversionMode,
                .MultiThreaded           = imguiConfig.MultiThreaded
            };
            return Rhi::CreateImGuiRenderer(imguiCreateDesc);
        }
        else
#endif
        {
            return Rhi::CreateImGuiNullRenderer();
        }
    }

    [[nodiscard]] static Rhi::ImGuiRendererCreateDesc GetImGuiCreateDesc(
        RhiGraphicsSubmodule* graphicsSubmodule)
    {
        Ptr<Rhi::IRhiDevice> rhiDevice;
        graphicsSubmodule->QueryInterface(Rhi::IID_RhiDevice, rhiDevice.DblPtr<IObject>());
    }

    GraphicsModule::GraphicsModule(
        IReferenceCounters*         counters,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_GraphicsModule),
        m_PlatformWindowSubmodule(CreateWindowSubmodule(config.RhiDeviceDesc)),
        m_RhiGraphicsSubmodule(ObjectAllocator<RhiGraphicsSubmodule>()(Rhi::CreateRhiDevice(config.RhiDeviceDesc))),
        m_RendererSubmodule(config.EnableRendererSubmodule ? ObjectAllocator<RendererSubmodule>()(m_RhiGraphicsSubmodule, config.SyncInterval) : nullptr),
        m_ImGuiSubmodule(ObjectAllocator<ImGuiSubmodule>()(CreateImGuiRenderer(m_RhiGraphicsSubmodule, config.ImGuiConfig, m_PlatformWindowSubmodule && m_RhiGraphicsSubmodule)))
    {
    }
} // namespace Ame