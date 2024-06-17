#include <ranges>

#include <Gfx/RenderGraph/Compositors/SpriteBatchCompositor.hpp>
#include <Gfx/Compositor/EntityCompositor.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>

namespace Ame::Gfx
{
    SpriteBatchCompositor ::SpriteBatchCompositor(
        EntityCompositor& entityCompositor) :
        m_OnRenderCompose(
            entityCompositor.OnRenderCompose(
                [this](Signals::Data::DrawCompositorData& renderData)
                {
                    OnRenderCompose(renderData);
                }))
    {
    }

    //

    struct SpriteEntityDrawInfo
    {
        CRef<EntityDrawInfo>          DrawInfo;
        const Ecs::Component::Sprite* Sprite;

        explicit operator bool() const
        {
            return Sprite != nullptr;
        }
    };

    static auto BuilderSpriteFilter(
        std::span<const EntityDrawInfo> entities)
    {
        return entities |
               std::views::transform(
                   [](const EntityDrawInfo& drawInfo)
                   {
                       return SpriteEntityDrawInfo{
                           .DrawInfo = drawInfo,
                           .Sprite   = drawInfo.Entity->get<Ecs::Component::Sprite>()
                       };
                   }) |
               std::views::filter(
                   [](const SpriteEntityDrawInfo& entity)
                   {
                       return static_cast<bool>(entity);
                   });
    }

    void SpriteBatchCompositor::OnRenderCompose(
        Signals::Data::DrawCompositorData& renderData)
    {
        auto allSprites = BuilderSpriteFilter(renderData.Entities);
        if (allSprites.empty())
        {
            return;
        }

        for (const auto& sprite : allSprites)
        {
            float distance = renderData.DistanceTo(sprite.DrawInfo);

        }

        //renderData.Compositor.Submit();
    }
} // namespace Ame::Gfx