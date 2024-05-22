#pragma once

#include <Gfx/RG/Resources/FrameResource.hpp>
#include <Gfx/RG/Resources/AABBBuffer.hpp>
#include <Gfx/RG/Resources/TransformBuffer.hpp>
#include <Gfx/RG/Resources/CameraCullResult.hpp>
#include <Gfx/RG/Ecs/System.hpp>

namespace Ame::Gfx::RG
{
    class CoreResources
    {
    public:
        CoreResources(
            Rhi::Device&   Device,
            Ecs::Universe& Universe);

    public:
        [[nodiscard]] const Rhi::Buffer&      GetFrameResource() const;
        [[nodiscard]] Rhi::BufferViewDesc     GetFrameResourceViewDesc() const;
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

    public:
        [[nodiscard]] uint32_t                  GetEntitiesCount() const;
        [[nodiscard]] EntityStore::RowGenerator GetEntities() const;

    public:
        [[nodiscard]] const AABBBuffer&      GetAABBBuffer() const;
        [[nodiscard]] const TransformBuffer& GetTransformBuffer() const;
        [[nodiscard]] const InstanceBuffer&  GetInstancesTableBuffer() const;

    public:
        [[nodiscard]] AABBBuffer&      GetAABBBuffer();
        [[nodiscard]] TransformBuffer& GetTransformBuffer();
        [[nodiscard]] InstanceBuffer&  GetInstancesTableBuffer();

    public:
        /// <summary>
        /// Update frame resource for the current frame
        /// </summary>
        void UpdateFrameResource(
            float                        EngineTime,
            float                        GameTime,
            float                        DeltaTime,
            const Ecs::Entity&           CameraEntity,
            const Math::TransformMatrix& Transform,
            const Math::Matrix4x4&       Projection,
            const Math::Vector2&         Viewport);

    public:
        /// <summary>
        /// Reset camera storage resources
        /// </summary>
        void ResetCameraStorage();

        /// <summary>
        /// Collect entities for rendering
        /// </summary>
        void CollectEntities();

    private:
        /// <summary>
        /// Collect entities for rendering
        /// </summary>
        void CollectEntities(
            const CameraRenderRule& RenderRule);

    private:
        /// <summary>
        /// Allocate the frame resource
        /// </summary>
        [[nodiscard]] static Rhi::Buffer AllocateFrameResource(
            Rhi::Device& Device);

    private:
        Ref<Rhi::Device>   m_Device;
        Ref<Ecs::Universe> m_Universe;

        Rhi::Buffer      m_FrameResourceBuffer;
        FrameResourceCPU m_FrameResource;

        AABBBuffer       m_AABBBuffer;
        TransformBuffer  m_TransformBuffer;
        CameraCullResult m_CameraCullResult;

        EcsSystemHooks m_EcsSystemHooks;
    };
} // namespace Ame::Gfx::RG