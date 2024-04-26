#include <Gfx/RG/Graph.hpp>

namespace Ame::Gfx::RG
{
    Graph::Graph(
        FrameTimer&  Timer,
        Rhi::Device& Device) :
        m_Timer(Timer),
        m_Device(Device),
        m_Context(Device)
    {
    }

    void Graph::Update()
    {
        m_Builder.Build(m_Context);
    }

    void Graph::UpdateFrameStorage(
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        m_Context.UpdateFrameStorage(
            static_cast<float>(m_Timer.get().GetEngineTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            static_cast<float>(m_Timer.get().GetDeltaTime()),
            Transform,
            Projection,
            Viewport);
    }

    void Graph::Execute()
    {
        m_Context.Execute();
        m_Builder.RemoveOneShotPasses();
    }

    //

    Builder& Graph::GetBuilder()
    {
        return m_Builder;
    }

    const Storage& Graph::GetStorage() const
    {
        return m_Context.GetStorage();
    }

    Storage& Graph::GetStorage()
    {
        return m_Context.GetStorage();
    }
} // namespace Ame::Gfx::RG