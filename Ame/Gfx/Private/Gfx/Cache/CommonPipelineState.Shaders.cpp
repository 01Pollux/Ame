#include <ranges>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    auto CommonPipelineState::ShaderTable::Create(
        ShaderTaskStorage tasks) -> Co::result<ShaderTable>
    {
        ShaderTable table;

        table.Shaders.reserve(tasks.size());
        table.ShaderDescs.reserve(tasks.size());

        for (auto& shader : tasks)
        {
            table.Shaders.emplace_back(co_await shader);
            table.ShaderDescs.emplace_back(table.Shaders.back().GetDesc());
        }

        co_return table;
    }

    //

    auto CommonPipelineState::PrepareShaders(
        Type type) -> Co::result<ShaderTaskStorage>
    {
        ShaderTaskStorage tasks;

        switch (type)
        {
        case Type::EntityCollectPass:
        {
            tasks.emplace_back(m_CommonShaders.get().Load(CommonShader::Type::EntityCollectPass_CS));
            break;
        }

        default:
            std::unreachable();
        }

        co_return tasks;
    }
} // namespace Ame::Gfx::Cache
