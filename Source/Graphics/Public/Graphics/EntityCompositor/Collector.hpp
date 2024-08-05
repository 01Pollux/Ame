#pragma once

#include <Graphics/EntityCompositor/Core.hpp>

namespace Ame::Gfx
{
    class EntityCollector
    {
        friend class EntityDrawer;

    public:
        EntityCollector(EntityStorage& storage) :
            m_Storage(storage)
        {
        }

    private:
        void Reset();
        void AddEntity(
            DrawInstanceType      type,
            Ecs::IBaseRenderable* renderable,
            uint32_t              instanceId);

    private:
        void Sort(
            const Math::TransformMatrix& cameraTransform);
        void Upload(
            RG::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);

    private:
        [[nodiscard]] Rhi::IRhiDevice* GetRenderDevice();

    private:
        Ref<EntityStorage> m_Storage;

        EntityDrawCommandsCategory m_DrawCommands;
    };
} // namespace Ame::Gfx