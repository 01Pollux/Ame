#pragma once

#include <boost/container/flat_set.hpp>

#include <Rhi/Resource/PipelineState.hpp>

#include <Gfx/RG/Resources/VertexBuffer.hpp>
#include <Gfx/RG/Resources/IndexBuffer.hpp>
#include <Gfx/RG/Resources/InstanceBuffer.hpp>

namespace Ame::Ecs::Component
{
    struct Transform;
    struct BaseRenderable;
} // namespace Ame::Ecs::Component

namespace Ame::Gfx::RG
{
    struct CameraCullDesc
    {
        Rhi::Util::BlockBasedBufferDesc VertexDesc;
        Rhi::Util::BlockBasedBufferDesc IndexDesc;
        Rhi::Util::SlotBasedBufferDesc  InstanceDesc;
        // Estimated number of unique objects per camera
        uint32_t EstimatedRowSize       = 1024;
        uint32_t EstimatedEntitiesCount = 1024 * 4;
        uint32_t EstimatedCameraCount   = 2;
    };

    /// <summary>
    /// Contains the result of culling a camera.
    /// For each frame, the camera culler will process all objects visible by the camera (mask, cull, etc.)
    /// and will return a CameraCullResult containing the data needed to render the objects.
    ///
    /// The CameraCullResult will be later used for indirect rendering.
    /// </summary>
    class CameraCullResult
    {
        struct StagedEntity
        {
            Ref<const Ecs::Component::BaseRenderable> Renderable;
            Ref<RenderInstance>                       Instance;
            float                                     Distance;

            std::partial_ordering operator<=>(const StagedEntity& Other) const noexcept;
        };

        struct CameraStorage
        {
            VertexBuffer   DynamicVertices;
            IndexBuffer    DynamicIndices;
            InstanceBuffer AllInstances;

            CameraStorage(
                Rhi::Device&                           RhiDevice,
                const Rhi::Util::BlockBasedBufferDesc& VertexDesc,
                const Rhi::Util::BlockBasedBufferDesc& IndexDesc,
                const Rhi::Util::SlotBasedBufferDesc&  InstanceDesc);

            void Reset();
        };

    public:
        struct Row
        {
            Rhi::Buffer             VertexBuffer;
            Rhi::Buffer             IndexBuffer;
            Ptr<Rhi::PipelineState> PipelineState;
            uint32_t                Count = 1;
        };

    public:
        CameraCullResult(
            Rhi::Device&          RhiDevice,
            const CameraCullDesc& Desc = {});

        /// <summary>
        /// Reset cameras storages
        /// </summary>
        void Reset();

    public:
        /// <summary>
        /// Add an entity to the cull result.
        /// </summary>
        void AddEntity(
            float                                 Distance,
            const Ecs::Component::BaseRenderable& Renderable,
            RenderInstance&                       Instance);

        /// <summary>
        /// Sort the cull result.
        /// All entities are reduced to a single buffer and sorted by effective distance, pipelinestate, distance, vertexbuffer, indexbuffer.
        /// </summary>
        void Upload();

    private:
        /// <summary>
        /// Reset the cull result.
        /// </summary>
        void PrepareForUpload();

    private:
        using Data           = std::vector<Row>;
        using CameraStorages = std::vector<CameraStorage>;
        using FlatEntities   = boost::container::flat_multiset<StagedEntity>;

        Ref<Rhi::Device> m_Device;

        Rhi::Util::BlockBasedBufferDesc m_CameraVertexDesc;
        Rhi::Util::BlockBasedBufferDesc m_CameraIndexDesc;
        Rhi::Util::SlotBasedBufferDesc  m_CameraInstanceDesc;

        CameraStorages m_Cameras;
        FlatEntities   m_StagedEntities;
        Data           m_Rows;
        int8_t         m_CurrentCamera = -1;
    };
} // namespace Ame::Gfx::RG