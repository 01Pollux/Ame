#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonShader.hpp>
#include <Gfx/Cache/CommonShader.ShaderGuids.hpp>

#include <Asset/Storage.hpp>
#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Cache
{
    using ShaderSourceAsset = Ptr<Asset::Gfx::ShaderSourceAsset>;

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
        Co::executor&          Executor,
        Asset::Storage&        Storage,
        const Asset::Handle&   Handle,
        Rhi::ShaderCompileDesc Desc)
    {
        auto Shader = co_await LoadShader({}, Executor, Storage, Handle);
        if (!Shader)
        {
            Log::Renderer().Error("Failed to load shader: {}", Handle.ToString());
            co_return Rhi::ShaderBytecode{};
        }

        co_return co_await Shader->Load(Desc);
    }

    //

    Co::result<Rhi::ShaderBytecode> CommonShader::Load(
        Type ShaderType)
    {
        auto Index = std::to_underlying(ShaderType);
        if (!m_Caches[Index])
        {
            auto                  Executor = m_Runtime.get().background_executor();
            Co::scoped_async_lock Lock     = co_await m_Mutex.lock(Executor);
            if (!m_Caches[Index])
            {
                m_Caches[Index] = co_await Create(*Executor, m_AssetStorage, ShaderType);
            }
        }
        co_return m_Caches[Index].Borrow();
    }

    Co::result<Rhi::ShaderBytecode> CommonShader::Create(
        Co::executor&   Executor,
        Asset::Storage& AssetStorage,
        Type            ShaderType)
    {
        using namespace EnumBitOperators;

        Asset::Handle          ShaderGuid;
        Rhi::ShaderCompileDesc ShaderDesc;

        switch (ShaderType)
        {
        case Type::EntityCollectPass_CS:
        {
            ShaderGuid = Asset::Handle::FromString(ShaderGuids::s_EntityCollectPass);
            ShaderDesc.SetStage(Rhi::ShaderType::COMPUTE_SHADER);
            break;
        }

        case Type::GBufferPass_PS:
        {
            ShaderGuid = Asset::Handle::FromString(ShaderGuids::s_BufferCollectPass);
            ShaderDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);
            ShaderDesc.Flags |= Rhi::ShaderCompileFlags::LibraryShader;
            break;
        }

        default:
            std::unreachable();
        }

        co_return co_await LoadAndCompile({}, Executor, AssetStorage, ShaderGuid, std::move(ShaderDesc));
    }
} // namespace Ame::Gfx::Cache
