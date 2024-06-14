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
        const DrawInstanceOrder& instanceOrder,
        DrawInstanceType         type)
    {
        m_Compositor.get().Submit(instanceOrder, type);
    }
} // namespace Ame::Gfx