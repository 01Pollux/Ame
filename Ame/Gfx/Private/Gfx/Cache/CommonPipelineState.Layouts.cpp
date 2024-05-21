#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::PipelineLayout>> CommonPipelineState::PrepareLayout(
        Type PipelineType)
    {
        return m_CommonLayouts.get().Load(GetLayoutType(PipelineType));
    }

    CommonPipelineLayout::Type CommonPipelineState::GetLayoutType(
        Type PipelineType)
    {
        switch (PipelineType)
        {
        case Type::EntityCollectPass:
            return CommonPipelineLayout::Type::EntityCollectPass;

        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache
