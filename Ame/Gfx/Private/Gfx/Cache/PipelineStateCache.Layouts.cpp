#include <ranges>

#include <Gfx/Cache/PipelineStateCache.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineLayout>> PipelineStateCache::PrepareLayout(
        Type PipelineType)
    {
        return m_LayoutCache.get().Load(GetLayoutType(PipelineType));
    }

    PipelineLayoutCache::Type PipelineStateCache::GetLayoutType(
        Type PipelineType)
    {
        switch (PipelineType)
        {
        case Type::EntityCollectPass:
            return PipelineLayoutCache::Type::EntityCollectPass;

        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache
