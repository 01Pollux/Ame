#include <Rg/Graph.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rg
{
    void Graph::Execute()
    {
        auto rhiDevice = m_Context.GetStorage().GetDevice();
        auto context   = rhiDevice->GetImmediateContext();
        Execute({ &context, 1 });
    }

    void Graph::Execute(
        std::span<Dg::IDeviceContext*> contexts)
    {
        auto& resourceStorage = GetResourceStorage();

        if (resourceStorage.NeedsRebuild() ||
            m_Passes.NeedsRebuild())
        {
            resourceStorage.SetRebuildState(false);
            m_Passes.Build(m_Context);
        }
        m_Context.Execute(contexts);
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
} // namespace Ame::Rg