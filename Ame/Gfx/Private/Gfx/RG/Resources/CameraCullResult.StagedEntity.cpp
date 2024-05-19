#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    std::partial_ordering CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        auto MatA = Renderable.get().Material.get();
        auto MatB = Other.Renderable.get().Material.get();

        auto VtxA = Renderable.get().Vertex.RhiBuffer.Unwrap();
        auto VtxB = Other.Renderable.get().Vertex.RhiBuffer.Unwrap();

        auto IdxA = Renderable.get().Index.RhiBuffer.Unwrap();
        auto IdxB = Other.Renderable.get().Index.RhiBuffer.Unwrap();

        return std::tie(MatA, VtxA, IdxA, Distance) <=> std::tie(MatB, VtxB, IdxB, Other.Distance);
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
} // namespace Ame::Gfx::RG