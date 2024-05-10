#include <ranges>

#include <Gfx/Cache/PipelineStateCache.hpp>
#include <Rhi/Device/Device.hpp>

#include <Asset/Storage.hpp>
#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

namespace Ame::Gfx::Cache
{
    using ShaderSourceAsset = Ptr<Asset::Gfx::ShaderSourceAsset>;

    //

    [[nodiscard]] static Co::result<ShaderSourceAsset> LoadShader(
        Co::executor_tag,
        Co::executor&,
        Asset::Storage&      Storage,
        const Asset::Handle& Handle)
    {
        auto Asset = Storage.GetManager().LoadAsync(Handle, true);
        if (auto Shader = std::dynamic_pointer_cast<Asset::Gfx::ShaderSourceAsset>(co_await Asset))
        {
            co_return Shader;
        }
        co_return nullptr;
    }

    [[nodiscard]] static Co::result<Rhi::ShaderBytecode> LoadAndCompile(
        Co::executor_tag,
        Co::executor&                 Executor,
        Asset::Storage&               Storage,
        const Asset::Handle&          Handle,
        const Rhi::ShaderCompileDesc& Desc)
    {
        auto Shader = co_await LoadShader({}, Executor, Storage, Handle);
        co_return Shader ? (co_await Shader->Load(Desc)) : Rhi::ShaderBytecode{};
    }

    //

    auto PipelineStateCache::ShaderTable::Create(
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

    auto PipelineStateCache::PrepareShaders(
        Type PipelineType) -> Co::result<ShaderTaskStorage>
    {
        auto& Executor = *m_Runtime.get().background_executor();

        ShaderTaskStorage ShaderTasks;

        uint32_t ShaderIndex = std::to_underlying(PipelineType);

        switch (PipelineType)
        {
        case Type::EntityCollectPass:
        {
            auto ShaderGuid = Asset::Handle::FromString("c3fbdbaf-490b-4415-bc99-3b6a3ba84d6f");

            Rhi::ShaderCompileDesc ShaderDesc{
                .Stage = Rhi::ShaderType::COMPUTE_SHADER
            };

            ShaderTasks.emplace_back(LoadAndCompile({}, Executor, m_AssetStorage, ShaderGuid, ShaderDesc));
            break;
        }

        default:
            std::unreachable();
        }

        co_return ShaderTasks;
    }
} // namespace Ame::Gfx::Cache
