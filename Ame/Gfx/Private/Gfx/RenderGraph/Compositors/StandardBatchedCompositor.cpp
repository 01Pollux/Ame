#include <ranges>

#include <Gfx/RenderGraph/Compositors/StandardBatchedCompositor.hpp>
#include <Gfx/Compositor/EntityCompositor.hpp>
#include <Gfx/Shading/Material.hpp>

namespace Ame::Gfx
{
    namespace EC = Ecs::Component;

    StandardBatchedCompositor::StandardBatchedCompositor(
        EntityCompositor&                 entityCompositor,
        Cache::FrameDynamicBufferCache<>& bufferCache) :
        m_BufferCache(bufferCache),
        m_OnRenderCompose(
            entityCompositor.OnRenderCompose(
                [this](Signals::Data::DrawCompositorData& renderData)
                {
                    OnRenderCompose(renderData);
                }))
    {
    }

    //

    struct BatchEntityDrawInfo
    {
        CRef<EntityDrawInfo>           DrawInfo;
        const EC::BatchableRenderable* Entry;

        explicit operator bool() const
        {
            return Entry != nullptr;
        }
    };

    static auto BuilderSpriteFilter(
        std::span<const EntityDrawInfo> entities)
    {
        return entities |
               std::views::transform(
                   [](const EntityDrawInfo& drawInfo)
                   {
                       return BatchEntityDrawInfo{
                           .DrawInfo = drawInfo,
                           .Entry    = drawInfo.Entity->get<EC::BatchableRenderable>()
                       };
                   }) |
               std::views::filter(
                   [](const BatchEntityDrawInfo& entry)
                   {
                       return static_cast<bool>(entry);
                   });
    }

    void StandardBatchedCompositor::OnRenderCompose(
        Signals::Data::DrawCompositorData& renderData)
    {
        auto allBatchable = BuilderSpriteFilter(renderData.Entities);
        if (allBatchable.empty())
        {
            return;
        }

        for (const auto& [drawInfo, batchEntry] : allBatchable)
        {
            DrawInstanceOrder instanceOrder{
                .Instance{ .Material = batchEntry->Material },
                .Order = renderData.DistanceTo(drawInfo)
            };

            // if renderable has custom buffer, use it, else allocate it dynamically
            auto handle = m_BufferCache.get().Rent(
                std::bit_cast<const std::byte*>(batchEntry->VertexBuffer.data()),
                batchEntry->VertexBuffer.size_bytes());

            instanceOrder.Instance.VertexBuffer = m_BufferCache.get().GetBuffer(handle).Unwrap();
            instanceOrder.Instance.VertexOffset = handle.Offset;

            std::visit(
                [&](const auto& index)
                {
                    auto handle = m_BufferCache.get().Rent(std::bit_cast<const std::byte*>(index.data()), index.size_bytes());

                    instanceOrder.Instance.IndexBuffer = m_BufferCache.get().GetBuffer(handle).Unwrap();
                    instanceOrder.Instance.IndexOffset = handle.Offset;
                    instanceOrder.Instance.IndexCount  = static_cast<size_t>(index.size());

                    if constexpr (std::is_same_v<decltype(index), EC::BatchableRenderable::IndexView16>)
                    {
                        instanceOrder.Instance.IndexType = Rhi::IndexType::UINT16;
                    }
                    else if constexpr (std::is_same_v<decltype(index), EC::BatchableRenderable::IndexView32>)
                    {
                        instanceOrder.Instance.IndexType = Rhi::IndexType::UINT32;
                    }
                },
                batchEntry->IndexBuffer);

            auto             blendMode = batchEntry->Material->GetPipelineStateDesc().OutputMerger.RenderTarget.Mode;
            DrawInstanceType type;
            switch (blendMode)
            {
            case Shading::BlendMode::Opaque:
                type = DrawInstanceType::Opaque;
                break;
            case Shading::BlendMode::Transparent:
                type = DrawInstanceType::Transparent;
                break;
            case Shading::BlendMode::Overlay:
                type = DrawInstanceType::Overlay;
                break;
            }
            renderData.Compositor.Submit(instanceOrder, type);
        }
    }
} // namespace Ame::Gfx