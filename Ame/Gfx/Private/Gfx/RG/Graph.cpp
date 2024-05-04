#include <Gfx/RG/Graph.hpp>

namespace Ame::Gfx::RG
{
    Graph::Graph(
        FrameTimer&    Timer,
        Rhi::Device&   Device,
        Ecs::Universe& Universe) :
        m_Timer(Timer),
        m_Device(Device),
        m_Context(Device, Universe)
    {
    }

    void Graph::Update()
    {
        m_Passes.Build(m_Context);
        m_Context.Reset();
        m_Context.Update();
    }

    void Graph::UpdateFrameStorage(
        const Ecs::Entity&           CameraEntity,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        m_Context.UpdateFrameStorage(
            static_cast<float>(m_Timer.get().GetEngineTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            CameraEntity,
            Transform,
            Projection,
            Viewport);
    }

    void Graph::Execute()
    {
        m_Context.Execute();
        m_Passes.RemoveOneShotPasses();
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
} // namespace Ame::Gfx::RG