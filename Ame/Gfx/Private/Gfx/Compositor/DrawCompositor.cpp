#include <Gfx/Compositor/DrawCompositor.hpp>

namespace Ame::Gfx
{
    void DrawCompositor::Submit(
        const DrawInstanceOrder& instanceOrder)
    {
        m_Instances.emplace_back(instanceOrder);
    }

    void DrawCompositor::Sort()
    {
        std::ranges::sort(m_Instances, std::less<>{});
    }

    void DrawCompositor::Clear()
    {
        m_Instances.clear();
    }
} // namespace Ame::Gfx