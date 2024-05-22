#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    std::partial_ordering CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        auto MatA = Renderable.get().Material.get();
        auto MatB = Other.Renderable.get().Material.get();

        auto VtxA = Renderable.get().Vertex.NriBuffer;
        auto VtxB = Other.Renderable.get().Vertex.NriBuffer;

        auto IdxA = Renderable.get().Index.NriBuffer;
        auto IdxB = Other.Renderable.get().Index.NriBuffer;

        return std::tie(MatA, VtxA, IdxA, Distance) <=> std::tie(MatB, VtxB, IdxB, Other.Distance);
    }

    //

    CameraCullResult::StagedGroup::StagedGroup(
        EntityList   Group,
        nri::Buffer* VtxBuffer,
        nri::Buffer* IdxBuffer) :
        VtxBuffer(VtxBuffer),
        IdxBuffer(IdxBuffer),
        Entities(std::move(Group))
    {
        for (auto& Entity : Entities)
        {
            RMSDistance += Entity.Distance * Entity.Distance;
        }
        RMSDistance /= Entities.size();
    }

    Rhi::IndexType CameraCullResult::StagedGroup::GetIndexType() const
    {
        return Entities.front().Renderable.get().Index.Stride == sizeof(uint16_t) ? Rhi::IndexType::UINT16 : Rhi::IndexType::UINT32;
    }

    Ptr<Shading::Material> CameraCullResult::StagedGroup::GetMaterial() const
    {
        return Entities.front().Renderable.get().Material;
    }

    std::partial_ordering CameraCullResult::StagedGroup::operator<=>(
        const StagedGroup& Other) const noexcept
    {
        return RMSDistance <=> Other.RMSDistance;
    }
} // namespace Ame::Gfx::RG