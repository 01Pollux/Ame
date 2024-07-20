#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Graphics/GraphicsModule.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    GraphicsModule::GraphicsModule(
        IReferenceCounters*         counters,
        RhiModule*                  rhiModule,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_GraphicsModule),
        m_RendererSubmodule(ObjectAllocator<RendererSubmodule>()(rhiModule, config.SyncInterval))
    {
    }
} // namespace Ame