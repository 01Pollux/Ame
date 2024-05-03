#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    auto CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        bool VertexBuffereda = Renderable->Vertex.IsBuffered();
        bool VertexBufferedb = Other.Renderable->Vertex.IsBuffered();

        bool IndexBuffereda = Renderable->Index.IsBuffered();
        bool IndexBufferedb = Other.Renderable->Index.IsBuffered();

        auto a = std::tie(Renderable->PipelineState, VertexBuffereda, IndexBuffereda, Distance);
        auto b = std::tie(Other.Renderable->PipelineState, VertexBufferedb, IndexBufferedb, Other.Distance);
        auto c = a <=> b;

        return c;
    }

    //

    CameraCullResult::CameraCullResult(
        Rhi::Device&          RhiDevice,
        const CameraCullDesc& Desc) :
        m_DynamicVertices(RhiDevice, Desc.VertexDesc),
        m_DynamicIndices(RhiDevice, Desc.IndexDesc),
        m_AllInstances(RhiDevice, Desc.InstanceDesc)
    {
        m_Rows.reserve(Desc.EstimatedRowSize);
        m_StagedEntities.reserve(Desc.EstimatedEntitiesCount);
    }

    void CameraCullResult::AddEntity(
        float                                 Distance,
        const Ecs::Component::BaseRenderable& Renderable)
    {
        m_StagedEntities.emplace(&Renderable, Distance);
    }

    void CameraCullResult::Upload()
    {
        if (m_StagedEntities.empty())
        {
            return;
        }

        Reset();
        const Rhi::Buffer *LastVertexBuffer = nullptr,
                          *LastIndexBuffer  = nullptr;

        for (auto& [Renderable, Distance] : m_StagedEntities)
        {
            auto& Vertex = Renderable->Vertex;
            auto& Index  = Renderable->Index;

            bool NewRow = false;

            // If the vertex or index buffer is unique, create a new row
            if (Vertex.IsBuffered() || Index.IsBuffered())
            {
                NewRow = true;
            }
            else if (LastVertexBuffer != &Vertex.RhiBuffer || LastIndexBuffer != &Index.RhiBuffer)
            {
                NewRow           = true;
                LastVertexBuffer = &Vertex.RhiBuffer;
                LastIndexBuffer  = &Index.RhiBuffer;
            }

            if (NewRow)
            {
                m_Rows.emplace_back(Vertex.RhiBuffer, Index.RhiBuffer, Renderable->PipelineState);
            }
            else
            {
                m_Rows.back().Count++;
            }
        }
        m_StagedEntities.clear();
    }

    void CameraCullResult::Reset()
    {
        m_Rows.clear();
        m_Rows.reserve(m_StagedEntities.size());
    }
} // namespace Ame::Gfx::RG