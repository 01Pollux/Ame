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
            const Ecs::Component::BaseRenderable* Renderable;
            float                                 Distance;

            auto operator<=>(const StagedEntity& Other) const noexcept;
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

    public:
        /// <summary>
        /// Add an entity to the cull result.
        /// </summary>
        void AddEntity(
            float                                 Distance,
            const Ecs::Component::BaseRenderable& Renderable);

        /// <summary>
        /// Sort the cull result.
        /// All entities are reduced to a single buffer and sorted by effective distance, pipelinestate, distance, vertexbuffer, indexbuffer.
        /// </summary>
        void Upload();

    private:
        /// <summary>
        /// Reset the cull result.
        /// </summary>
        void Reset();

    private:
        using Data         = std::vector<Row>;
        using FlatEntities = boost::container::flat_multiset<StagedEntity>;

        VertexBuffer   m_DynamicVertices;
        IndexBuffer    m_DynamicIndices;
        InstanceBuffer m_AllInstances;

        Data         m_Rows;
        FlatEntities m_StagedEntities;
    };
} // namespace Ame::Gfx::RG