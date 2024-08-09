#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    EntityCollector::EntityCollector(
        EntityStorage& storage) :
        m_Storage(storage)
    {
        auto& world = *m_Storage.get().GetWorld();
        world->component<EntityDrawCommandsCategoryIterator>();
    }

    void EntityCollector::ResetCommands()
    {
        for (auto& commands : m_DrawCommands)
        {
            commands.clear();
        }
    }

    void EntityCollector::AddEntity(
        DrawInstanceType      type,
        Ecs::IBaseRenderable* renderable,
        uint32_t              instanceId)
    {
        auto& commands = m_DrawCommands[std::to_underlying(type)];
        commands.emplace_back(renderable, instanceId);
    }

    //

    void EntityCollector::Sort(
        const Math::TransformMatrix& cameraTransform)
    {
        for (auto& commands : m_DrawCommands)
        {
            std::sort(
                commands.begin(), commands.end(),
                [&cameraTransform](auto& a, auto& b)
                {
                    a.SetDistance(cameraTransform.GetPosition());
                    b.SetDistance(cameraTransform.GetPosition());
                    return a < b;
                });
        }
    }

    void EntityCollector::Upload(
        RG::Graph&                       cameraGraph,
        const CameraFrameDataUpdateDesc& frameData)
    {
        m_Storage.get().WriteInstanceIndices(m_DrawCommands);
        m_Storage.get().UploadToRenderGraph(cameraGraph, frameData);

        auto& world = *m_Storage.get().GetWorld();
        world->emplace<EntityDrawCommandsCategoryIterator>(m_DrawCommands);
    }

    void EntityCollector::Reset()
    {
        auto& world = *m_Storage.get().GetWorld();
        world->remove<EntityDrawCommandsCategoryIterator>();
    }

    //

    Rhi::IRhiDevice* EntityCollector::GetRenderDevice()
    {
        return m_Storage.get().GetRenderDevice();
    }

    Ecs::World* EntityCollector::GetWorld()
    {
        return m_Storage.get().GetWorld();
    }
} // namespace Ame::Gfx