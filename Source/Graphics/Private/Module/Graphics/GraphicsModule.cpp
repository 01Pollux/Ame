#include <Module/Graphics/GraphicsModule.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    [[nodiscard]] static PlatformWindowSubmodule* CreateWindowSubmodule(
        const Rhi::DeviceCreateDesc& desc)
    {
        return (desc.Surface.has_value() && desc.Surface->Window) ? ObjectAllocator<PlatformWindowSubmodule>()(desc.Surface->Window) : nullptr;
    }

    GraphicsModule::GraphicsModule(
        IReferenceCounters*         counters,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_GraphicsModule),
        m_PlatformWindowSubmodule(CreateWindowSubmodule(config.RhiDeviceDesc)),
        m_RhiGraphicsSubmodule(ObjectAllocator<RhiGraphicsSubmodule>()(Rhi::CreateRhiDevice(config.RhiDeviceDesc))),
        m_ImGuiSubmodule((m_PlatformWindowSubmodule && m_RhiGraphicsSubmodule && config.EnableImGuiSubmodule) ? ObjectAllocator<ImGuiSubmodule>()() : nullptr)
    {
    }
} // namespace Ame