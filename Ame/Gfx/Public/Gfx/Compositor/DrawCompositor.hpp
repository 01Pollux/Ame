#pragma once

#include <Gfx/RenderGraph/Resources/DrawInstance.hpp>

namespace Ame::Gfx
{
    class DrawCompositor
    {
        using DrawInstanceList  = std::vector<DrawInstanceOrder>;
        using DrawInstanceLists = std::array<DrawInstanceList, static_cast<size_t>(DrawInstanceType::Count)>;

    public:
        /// <summary>
        /// Submit a render instance to the compositor.
        /// </summary>
        /// <param name="instanceOrder"></param>
        void Submit(
            const DrawInstanceOrder& instanceOrder,
            DrawInstanceType         type);

        /// <summary>
        /// Sort the render instances.
        /// </summary>
        void Sort();

        /// <summary>
        /// Clear the render instances.
        /// </summary>
        void Clear();

    private:
        DrawInstanceLists m_Instances;
    };
} // namespace Ame::Gfx