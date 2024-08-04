#include <numeric>

#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

namespace Ame::Gfx
{
    void EntityCollector::Reset()
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
    }
} // namespace Ame::Gfx