#include <numeric>

#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

namespace Ame::Gfx
{
    void EntityCollector::AddEntity(
        DrawInstanceType      type,
        Ecs::IBaseRenderable* renderable,
        EntityDrawInstance    instance)
    {
        auto& commands = m_DrawCommands[std::to_underlying(type)];
        commands.emplace_back(renderable, instance);
    }

    void EntityCollector::Reset()
    {
        m_TotalDrawCount = 0;
        for (auto& commands : m_DrawCommands)
        {
            commands.clear();
        }
    }

    void EntityCollector::Finalize()
    {
        for (auto& commands : m_DrawCommands)
        {
            m_TotalDrawCount += commands.size();
        }

        std::vector<EntityDrawInstance> drawInstances;
        drawInstances.reserve(m_TotalDrawCount);

        for (auto& commands : m_DrawCommands)
        {
            for (auto& command : commands)
            {
                drawInstances.emplace_back(command.DrawInstance);
            }
        }

        m_Storage.get().WriteInstances(drawInstances);
    }

    void EntityCollector::Sort(
        const Math::TransformMatrix& cameraTransform)
    {
        for (auto [commands, sortedCommands] : std::views::zip(m_DrawCommands, m_SortedDrawCommands))
        {
            if (sortedCommands.empty() || sortedCommands.size() != commands.size())
            {
                sortedCommands.reserve(commands.size());
                uint32_t lowest = sortedCommands.size();
                std::iota(sortedCommands.begin(), sortedCommands.end(), lowest);
            }

            std::sort(sortedCommands.begin(), sortedCommands.end(),
                      [&commands, &cameraTransform](uint32_t a, uint32_t b)
                      {
                          commands[a].SetDistance(cameraTransform.GetPosition());
                          commands[b].SetDistance(cameraTransform.GetPosition());
                          return commands[a].Distance < commands[b].Distance;
                      });

            m_TotalDrawCount += sortedCommands.size();
        }
    }

    void EntityCollector::Upload(
        RG::Graph&                       cameraGraph,
        const CameraFrameDataUpdateDesc& frameData)
    {
        m_Storage.get().WriteInstanceIndices(m_SortedDrawCommands);
        m_Storage.get().ReserveIndirectBuffers(m_TotalDrawCount);
        m_Storage.get().UploadToRenderGraph(cameraGraph, frameData);
    }
} // namespace Ame::Gfx