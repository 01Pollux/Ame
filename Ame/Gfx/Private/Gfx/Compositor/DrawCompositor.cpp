#include <Gfx/Compositor/DrawCompositor.hpp>

namespace Ame::Gfx
{
    void DrawCompositor::Submit(
        const DrawInstanceOrder& instanceOrder,
        DrawInstanceType         type)
    {
        size_t index = static_cast<size_t>(type);
        m_Instances[index].emplace_back(instanceOrder);
    }

    //

    void DrawCompositor::Sort()
    {
        for (auto& instanceList : m_Instances)
        {
            std::ranges::sort(instanceList, std::less<>{});
        }
    }

    void DrawCompositor::Clear()
    {
        for (auto& instanceList : m_Instances)
        {
            instanceList.clear();
        }
    }
} // namespace Ame::Gfx