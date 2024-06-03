#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Gfx/Shading/Material.hpp>

namespace Ame::Gfx::RG
{
    std::partial_ordering CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& other) const noexcept
    {
        auto materialA = Renderable.get().Material->GetPipelineHash();
        auto materialB = other.Renderable.get().Material->GetPipelineHash();

        auto vertexA = Renderable.get().Vertex.NriBuffer;
        auto vertexB = other.Renderable.get().Vertex.NriBuffer;

        auto indexA = Renderable.get().Index.NriBuffer;
        auto indexB = other.Renderable.get().Index.NriBuffer;

        return std::tie(materialA, vertexA, indexA, Distance) <=>
               std::tie(materialB, vertexB, indexB, other.Distance);
    }

    //

    CameraCullResult::StagedGroup::StagedGroup(
        EntityList   entityList,
        nri::Buffer* vertexBuffer,
        nri::Buffer* indexBuffer) :
        VtxBuffer(vertexBuffer),
        IdxBuffer(indexBuffer),
        Entities(entityList)
    {
    }

    void CameraCullResult::StagedGroup::CalculateRMS()
    {
        for (auto& entity : Entities)
        {
            RMSDistance += entity.Distance * entity.Distance;
        }
        RMSDistance /= Entities.size();
    }

    Rhi::IndexType CameraCullResult::StagedGroup::GetIndexType() const
    {
        auto& firstRenderable = Entities.front().Renderable.get();
        return firstRenderable.GetIndexType();
    }

    Ptr<Shading::Material> CameraCullResult::StagedGroup::GetMaterial() const
    {
        auto& firstRenderable = Entities.front().Renderable.get();
        return firstRenderable.Material;
    }

    std::partial_ordering CameraCullResult::StagedGroup::operator<=>(
        const StagedGroup& Other) const noexcept
    {
        return RMSDistance <=> Other.RMSDistance;
    }
} // namespace Ame::Gfx::RG