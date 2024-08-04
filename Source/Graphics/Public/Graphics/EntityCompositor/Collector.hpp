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
        void Reset();
        void AddEntity(
            DrawInstanceType      type,
            Ecs::IBaseRenderable* renderable,
            uint32_t              instanceId);

    public:
        void Sort(
            const Math::TransformMatrix& cameraTransform);
        void Upload(
            RG::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);

    private:
        Ref<EntityStorage> m_Storage;

        EntityDrawCommandsCategory m_DrawCommands;
    };
} // namespace Ame::Gfx