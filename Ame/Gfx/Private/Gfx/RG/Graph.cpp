#include <Gfx/RG/Graph.hpp>

namespace Ame::Gfx::RG
{
    Graph::Graph(
        FrameTimer&    frameTimer,
        Rhi::Device&   rhiDevice,
        Ecs::Universe& universe) :
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Context(rhiDevice, universe)
    {
    }

    void Graph::Update()
    {
        m_Passes.Build(m_Context);
        m_Context.Reset();
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