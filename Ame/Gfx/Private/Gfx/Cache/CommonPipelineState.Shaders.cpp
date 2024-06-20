#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    auto CommonPipelineState::PrepareShaders(
        Type type) -> ShaderTable
    {
        ShaderTable table;

        switch (type)
        {
        case Type::EntityCollectPass:
        {
            table.Shaders.emplace_back(m_CommonShaders.get().Load(CommonShader::Type::EntityCollectPass_CS).get());
            break;
        }

        default:
            std::unreachable();
        }

        table.ShaderDescs.reserve(table.Shaders.size());
        for (auto& shader : table.Shaders)
        {
            table.ShaderDescs.emplace_back(shader.GetDesc());
        }

        return table;
    }
} // namespace Ame::Gfx::Cache
