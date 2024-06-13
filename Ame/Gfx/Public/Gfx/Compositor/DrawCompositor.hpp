#pragma once

#include <Gfx/RenderGraph/Resources/DrawInstance.hpp>

namespace Ame::Gfx
{
    class DrawCompositor
    {
    public:
        /// <summary>
        /// Submit a render instance to the compositor.
        /// </summary>
        /// <param name="instanceOrder"></param>
        void Submit(
            const DrawInstanceOrder& instanceOrder);

        /// <summary>
        /// Sort the render instances.
        /// </summary>
        void Sort();

        /// <summary>
        /// Clear the render instances.
        /// </summary>
        void Clear();

    private:
        std::vector<DrawInstanceOrder> m_Instances;
    };
} // namespace Ame::Gfx