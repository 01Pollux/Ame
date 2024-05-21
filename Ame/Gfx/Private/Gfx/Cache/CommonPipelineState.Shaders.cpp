#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    auto CommonPipelineState::ShaderTable::Create(
        ShaderTaskStorage Tasks) -> Co::result<ShaderTable>
    {
        ShaderTable Table;

        Table.Shaders.reserve(Tasks.size());
        Table.ShaderDescs.reserve(Tasks.size());

        for (auto& Shader : Tasks)
        {
            Table.Shaders.emplace_back(co_await Shader);
            Table.ShaderDescs.emplace_back(Table.Shaders.back().GetDesc());
        }

        co_return Table;
    }

    //

    auto CommonPipelineState::PrepareShaders(
        Type PipelineType) -> Co::result<ShaderTaskStorage>
    {
        ShaderTaskStorage ShaderTasks;

        switch (PipelineType)
        {
        case Type::EntityCollectPass:
        {
            ShaderTasks.emplace_back(m_CommonShaders.get().Load(CommonShader::Type::EntityCollectPass_CS));
            break;
        }

        default:
            std::unreachable();
        }

        co_return ShaderTasks;
    }
} // namespace Ame::Gfx::Cache
