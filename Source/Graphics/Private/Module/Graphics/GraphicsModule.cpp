#include <Module/Graphics/GraphicsModule.hpp>

namespace Ame
{
    GraphicsModule::GraphicsModule(
        IReferenceCounters*         counters,
        Dependencies                dependencies,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_GraphicsModule),
        m_RendererSubmodule(ObjectAllocator<RendererSubmodule>()(dependencies.RhiMod, dependencies.EntMod, config))
    {
    }
} // namespace Ame