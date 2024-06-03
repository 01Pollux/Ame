#pragma once

#include <boost/container/flat_set.hpp>

#include <Gfx/RG/Resources/VertexBuffer.hpp>
#include <Gfx/RG/Resources/IndexBuffer.hpp>
#include <Gfx/RG/Resources/InstanceBuffer.hpp>

#include <Gfx/RG/EntityStore.hpp>

namespace Ame::Ecs::Component
{
    struct Transform;
    struct BaseRenderable;
} // namespace Ame::Ecs::Component

namespace Ame::Gfx::RG
{
    struct CameraCullDesc
    {
        Rhi::Util::BlockBasedBufferDesc VertexDesc{
            .UsageFlags = Rhi::BufferUsageBits::VERTEX_BUFFER
        };
        Rhi::Util::BlockBasedBufferDesc IndexDesc{
            .UsageFlags = Rhi::BufferUsageBits::INDEX_BUFFER
        };
        Rhi::Util::SlotBasedBufferDesc InstanceDesc{
            .UsageFlags = Rhi::BufferUsageBits::SHADER_RESOURCE
        };
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
            CRef<Ecs::Component::BaseRenderable> Renderable;
            Ref<RenderInstance>                  Instance;
            float                                Distance;

            std::partial_ordering operator<=>(const StagedEntity& other) const noexcept;
        };

        struct StagedGroup
        {
            using EntityList = std::span<const StagedEntity>;

            nri::Buffer* VtxBuffer;
            nri::Buffer* IdxBuffer;
            EntityList   Entities;
            double       RMSDistance = 0.;

            StagedGroup(
                std::span<const StagedEntity> entityList,
                nri::Buffer*                  vertexBuffer,
                nri::Buffer*                  indexBuffer);

            void                                 CalculateRMS();
            [[nodiscard]] Rhi::IndexType         GetIndexType() const;
            [[nodiscard]] Ptr<Shading::Material> GetMaterial() const;

            std::partial_ordering operator<=>(const StagedGroup& other) const noexcept;
        };

        struct CameraStorage
        {
            VertexBuffer   DynamicVertices;
            IndexBuffer    DynamicIndices;
            InstanceBuffer AllInstances;

            CameraStorage(
                Rhi::Device&                           rhiDevice,
                const Rhi::Util::BlockBasedBufferDesc& vertexDesc,
                const Rhi::Util::BlockBasedBufferDesc& indexDesc,
                const Rhi::Util::SlotBasedBufferDesc&  instanceDesc);

            void Reset();
            void Flush();
        };

    public:
        CameraCullResult(
            Rhi::Device&          rhiDevice,
            const CameraCullDesc& desc = {});

    public:
        [[nodiscard]] uint32_t                  GetEntitiesCount() const;
        [[nodiscard]] EntityStore::RowGenerator GetEntities() const;
        [[nodiscard]] const InstanceBuffer&     GetInstancesTableBuffer() const;
        [[nodiscard]] InstanceBuffer&           GetInstancesTableBuffer();

    public:
        /// <summary>
        /// Reset cameras storages
        /// </summary>
        void Reset();

    public:
        /// <summary>
        /// Add an entity to the cull result.
        /// </summary>
        void AddEntity(
            float                                 distance,
            const Ecs::Component::BaseRenderable& renderable,
            RenderInstance&                       instance);

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

        /// <summary>
        /// Cut the rows into subrows to fit vertex/index buffer limits if needed.
        /// Group the rows for upload.
        /// </summary>
        void StageUpload();

        /// <summary>
        /// Finalize the cull result into rows.
        /// </summary>
        void FinalizeStaging();

    private:
        using DataList           = std::vector<EntityStore::Row>;
        using CameraStorages     = std::vector<CameraStorage>;
        using FlatStagedEntities = boost::container::flat_multiset<StagedEntity>;
        using FlatStagedGroups   = std::vector<StagedGroup>;

        Ref<Rhi::Device> m_Device;

        Rhi::Util::BlockBasedBufferDesc m_CameraVertexDesc;
        Rhi::Util::BlockBasedBufferDesc m_CameraIndexDesc;
        Rhi::Util::SlotBasedBufferDesc  m_CameraInstanceDesc;

        FlatStagedEntities m_StagedEntities;
        FlatStagedGroups   m_StagedGroups;

        CameraStorages m_Cameras;
        DataList       m_Rows;
        int8_t         m_CurrentCamera = -1;

        uint32_t m_EntitiesCount = 0;
    };
} // namespace Ame::Gfx::RG