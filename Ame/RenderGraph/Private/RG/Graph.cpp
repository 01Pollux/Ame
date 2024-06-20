#include <RG/Graph.hpp>

namespace Ame::RG
{
    Graph::Graph(
        FrameTimer&  frameTimer,
        Rhi::Device& rhiDevice) :
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Context(rhiDevice)
    {
    }

    void Graph::Update()
    {
        m_Passes.Build(m_Context);
        m_Context.Update();
    }

    void Graph::UpdateFrameStorage(
        const Ecs::Entity&           cameraEntity,
        const Math::TransformMatrix& transform,
        const Math::Matrix4x4&       projection,
        const Math::Vector2&         viewport)
    {
        m_Context.UpdateFrameStorage(
            static_cast<float>(m_Timer.get().GetEngineTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            cameraEntity,
            transform,
            projection,
            viewport);
    }

    void Graph::Execute()
    {
        m_Context.Execute();
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