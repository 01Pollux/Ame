#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    std::partial_ordering CameraCullResult::StagedEntity::operator<=>(
        const StagedEntity& Other) const noexcept
    {
        bool UniquerVertexa = Renderable.get().Vertex.HasUniqueBuffer();
        bool UniquerVertexb = Other.Renderable.get().Vertex.HasUniqueBuffer();

        bool UniqueIndexa = Renderable.get().Index.HasUniqueBuffer();
        bool UniqueIndexb = Other.Renderable.get().Index.HasUniqueBuffer();

        auto a = std::tie(Renderable.get().PipelineState, UniquerVertexa, UniqueIndexa, Distance);
        auto b = std::tie(Other.Renderable.get().PipelineState, UniquerVertexb, UniqueIndexb, Other.Distance);
        auto c = a <=> b;

        return c;
    }
} // namespace Ame::Gfx::RG