#include <Rg/Graph.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Window/Window.hpp>
#include <Window/WindowEventListener.hpp>

namespace Ame::Rg
{
    Graph::Graph(IReferenceCounters* counter, Rhi::IRhiDevice* rhiDevice) : Base(counter), m_Context(rhiDevice)
    {
        auto window = rhiDevice->GetWindow();
        if (window)
        {
            auto& eventListener   = window->GetEventListener();
            m_OnWindowSizeChanged = { eventListener.OnWindowSizeChanged.Connect([this, window](const Math::Size2I&)
                                                                                { MarkDirty(); }) };
        }
    } // namespace Ame::Rg

    void Graph::Execute()
    {
        auto rhiDevice = m_Context.GetStorage().GetDevice();
        Execute(rhiDevice->GetImmediateContext());
    }

    void Graph::Execute(Dg::IDeviceContext* context)
    {
        if (IsDirty())
        {
            auto& passStorage = GetResourceStorage();
            passStorage.SetRebuildState(false);
            passStorage.ClearResources();
            m_Passes.Build(m_Context);
        }
        m_Context.Execute(context);
    }

    void Graph::MarkDirty() noexcept
    {
        GetResourceStorage().SetRebuildState(true);
    }

    bool Graph::IsDirty() const noexcept
    {
        return GetResourceStorage().NeedsRebuild() || m_Passes.NeedsRebuild();
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