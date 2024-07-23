#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Graphics/GraphicsModule.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    GraphicsModule::GraphicsModule(
        IReferenceCounters*         counters,
        Dependencies                dependencies,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_GraphicsModule),
        m_RendererSubmodule(ObjectAllocator<RendererSubmodule>()(dependencies.RhiMod, config.SyncInterval))
    {
    }
} // namespace Ame