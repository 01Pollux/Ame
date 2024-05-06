#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    std::partial_ordering CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        auto PsoA = Renderable.get().PipelineState.get();
        auto PsoB = Other.Renderable.get().PipelineState.get();

        auto VtxA = Renderable.get().Vertex.RhiBuffer.Unwrap();
        auto VtxB = Other.Renderable.get().Vertex.RhiBuffer.Unwrap();

        auto IdxA = Renderable.get().Index.RhiBuffer.Unwrap();
        auto IdxB = Other.Renderable.get().Index.RhiBuffer.Unwrap();

        return std::tie(PsoA, VtxA, IdxA, Distance) <=> std::tie(PsoB, VtxB, IdxB, Other.Distance);
    }

    //

    CameraCullResult::StagedGroup::StagedGroup(
        EntityList  Group,
        Rhi::Buffer VtxBuffer,
        Rhi::Buffer IdxBuffer) :
        VtxBuffer(std::move(VtxBuffer)),
        IdxBuffer(std::move(IdxBuffer)),
        Entities(std::move(Group))
    {
        for (auto& Entity : Entities)
        {
            RMSDistance += Entity.Distance * Entity.Distance;
        }
        RMSDistance /= Entities.size();
    }

    const Ecs::Component::BaseRenderable& CameraCullResult::StagedGroup::GetFirstRenderable() const
    {
        return Entities.front().Renderable;
    }

    const RenderInstance& CameraCullResult::StagedGroup::GetFirstInstance() const
    {
        return Entities.front().Instance;
    }

    std::partial_ordering CameraCullResult::StagedGroup::operator<=>(
        const StagedGroup& Other) const noexcept
    {
        return RMSDistance <=> Other.RMSDistance;
    }

    //

    CameraCullResult::Row::Row(
        StagedGroup Group) :
        VtxBuffer(std::move(Group.VtxBuffer)),
        IdxBuffer(std::move(Group.IdxBuffer)),
        PipelineState(Group.GetFirstRenderable().PipelineState),
        Count(Group.Entities.size())
    {
    }
} // namespace Ame::Gfx::RG