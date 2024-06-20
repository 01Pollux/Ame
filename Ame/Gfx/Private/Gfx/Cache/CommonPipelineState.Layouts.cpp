#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>
#include <Gfx/Cache/CommonPipelineLayout.hpp>

namespace Ame::Gfx::Cache
{
    Co::result<Ptr<Rhi::ScopedPipelineLayout>> CommonPipelineState::PrepareLayout(
        Type type)
    {
        return m_CommonLayouts.get().Load(GetLayoutType(type));
    }

    CommonPipelineLayout::Type CommonPipelineState::GetLayoutType(
        Type type)
    {
        switch (type)
        {
        case Type::EntityCollectPass:
            return CommonPipelineLayout::Type::EntityCollectPass;

        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache
