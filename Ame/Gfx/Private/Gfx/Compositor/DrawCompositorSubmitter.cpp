#include <Gfx/Compositor/DrawCompositor.hpp>
#include <Gfx/Compositor/DrawCompositorSubmitter.hpp>

namespace Ame::Gfx
{
    DrawCompositorSubmitter::DrawCompositorSubmitter(
        DrawCompositor& compositor) :
        m_Compositor(compositor)
    {
    }

    void DrawCompositorSubmitter::Submit(
        const DrawInstanceOrder& instanceOrder)
    {
        m_Compositor.get().Submit(instanceOrder);
    }
} // namespace Ame::Gfx