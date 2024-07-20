#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Graphics/GraphicsModule.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    RendererSubmodule::RendererSubmodule(
        IReferenceCounters*   counters,
        RhiGraphicsSubmodule* rhiGraphicsSubmodule,
        uint32_t              syncInterval) :
        Base(counters, IID_RendererSubmodule),
        m_SyncInterval(syncInterval)
    {
        rhiGraphicsSubmodule->QueryInterface(Rhi::IID_RhiDevice, m_RhiDevice.DblPtr<IObject>());
    }

    bool RendererSubmodule::Tick()
    {
        if (m_RhiDevice->BeginFrame()) [[likely]]
        {
            Invoke_OnRenderBegin();
            Invoke_OnRenderUpdate();
            Invoke_OnRenderPostUpdate();
            Invoke_OnRenderEnd();

            m_RhiDevice->AdvanceFrame(m_SyncInterval);
            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace Ame