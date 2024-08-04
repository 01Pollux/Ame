#pragma once

#include <Graphics/EntityCompositor/Core.hpp>

namespace Ame::Gfx
{
    class EntityCollector
    {
    public:
        EntityCollector(EntityStorage& storage) :
            m_Storage(storage)
        {
        }

    public:
        void AddEntity(
            DrawInstanceType      type,
            Ecs::IBaseRenderable* renderable,
            EntityDrawInstance    instance);

        void Reset();

        /// <summary>
        /// Upload common data to the GPU.
        /// </summary>
        void Finalize();

        void Sort(
            const Math::TransformMatrix& cameraTransform);

        /// <summary>
        /// Upload camera relative data to the GPU.
        /// </summary>
        void Upload(
            RG::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);

    private:
        Ref<EntityStorage> m_Storage;
        uint32_t           m_TotalDrawCount = 0;

        EntityDrawCommandsCategory       m_DrawCommands;
        SortedEntityDrawCommandsCategory m_SortedDrawCommands;
    };
} // namespace Ame::Gfx