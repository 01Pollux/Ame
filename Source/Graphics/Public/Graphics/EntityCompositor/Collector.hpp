#pragma once

#include <Graphics/EntityCompositor/Core.hpp>

namespace Ame::Gfx
{
    class EntityCollector
    {
        friend class EntityDrawer;

    public:
        EntityCollector(
            EntityStorage& storage);

    private:
        void ResetCommands();
        void AddEntity(
            DrawInstanceType      type,
            Ecs::IBaseRenderable* renderable,
            uint32_t              instanceId);
        void UpdateLights(
            std::span<const uint32_t> lightIds);

    private:
        void Sort(
            const Math::TransformMatrix& cameraTransform);
        void Upload(
            Rg::Graph&                       cameraGraph,
            const CameraFrameDataUpdateDesc& frameData);
        void Reset();

    private:
        [[nodiscard]] Rhi::IRhiDevice* GetRenderDevice();
        [[nodiscard]] Ecs::World*      GetWorld();

    private:
        Ref<EntityStorage> m_Storage;

        EntityDrawCommandsCategory m_DrawCommands;
    };
} // namespace Ame::Gfx