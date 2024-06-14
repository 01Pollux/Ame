#pragma once

#include <Gfx/RenderGraph/Resources/DrawInstance.hpp>

namespace Ame::Gfx
{
    class DrawCompositor;

    class DrawCompositorSubmitter
    {
    public:
        explicit DrawCompositorSubmitter(
            DrawCompositor& compositor);

        /// <summary>
        /// Submit a render instance to the compositor.
        /// </summary>
        void Submit(
            const DrawInstanceOrder& instanceOrder,
            DrawInstanceType         type);

    private:
        Ref<DrawCompositor> m_Compositor;
    };
} // namespace Ame::Gfx