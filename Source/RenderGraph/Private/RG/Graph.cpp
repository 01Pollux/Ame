#include <RG/Graph.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::RG
{
    void Graph::Execute(
        Rhi::IRhiDevice* rhiDevice)
    {
        auto context = rhiDevice->GetImmediateContext();
        Execute(rhiDevice, { &context, 1 });
    }

    void Graph::Execute(
        Rhi::IRhiDevice*               rhiDevice,
        std::span<Dg::IDeviceContext*> contexts)
    {
        auto  renderDevice    = rhiDevice->GetRenderDevice();
        auto& resourceStorage = GetResourceStorage();

        if (resourceStorage.NeedsRebuild() ||
            m_Passes.NeedsRebuild())
        {
            resourceStorage.SetRebuildState(false);
            m_Passes.Build(rhiDevice, m_Context);
        }
        m_Context.Execute(renderDevice, contexts);
    }

    //

    PassStorage& Graph::GetPassStorage()
    {
        return m_Passes;
    }

    const ResourceStorage& Graph::GetResourceStorage() const
    {
        return m_Context.GetStorage();
    }

    ResourceStorage& Graph::GetResourceStorage()
    {
        return m_Context.GetStorage();
    }
} // namespace Ame::RG