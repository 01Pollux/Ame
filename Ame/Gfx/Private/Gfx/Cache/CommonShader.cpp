#include <Rhi/Device/Device.hpp>
#include <Gfx/Cache/CommonShader.hpp>
#include <Gfx/Cache/CommonShader.ShaderGuids.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

#include <Asset/Storage.hpp>
#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Cache
{
    using ShaderSourceAsset = Ptr<Asset::Gfx::ShaderSourceAsset>;

    [[nodiscard]] static Co::result<ShaderSourceAsset> LoadShader(
        Co::executor_tag,
        Co::executor&,
        Asset::Storage& assetStorage,
        const Guid&     handle)
    {
        auto asset = assetStorage.GetManager().LoadAsync(handle, true);
        if (auto assetShader = std::dynamic_pointer_cast<Asset::Gfx::ShaderSourceAsset>(co_await asset))
        {
            co_return assetShader;
        }
        co_return nullptr;
    }

    [[nodiscard]] static Co::result<Rhi::ShaderBytecode> LoadAndCompile(
        Co::executor_tag,
        Co::executor&          executor,
        Asset::Storage&        storage,
        Guid                   guid,
        Rhi::ShaderCompileDesc desc)
    {
        auto assetShader = co_await LoadShader({}, executor, storage, guid);
        if (!assetShader)
        {
            Log::Gfx().Error("Failed to load shader: {}", guid.ToString());
            co_return Rhi::ShaderBytecode{};
        }

        co_return co_await assetShader->Load(desc);
    }

    //

    Co::result<Rhi::ShaderBytecode> CommonShader::Load(
        Type type)
    {
        auto index = std::to_underlying(type);
        if (!m_Caches[index])
        {
            auto                  Executor = m_Runtime.get().background_executor();
            Co::scoped_async_lock Lock     = co_await m_Mutex.lock(Executor);
            if (!m_Caches[index])
            {
                m_Caches[index] = co_await Create(*Executor, m_AssetStorage, type);
            }
        }
        co_return m_Caches[index].Borrow();
    }

    Co::result<Rhi::ShaderBytecode> CommonShader::Create(
        Co::executor&   executor,
        Asset::Storage& assetStorage,
        Type            type)
    {
        using namespace EnumBitOperators;

        Guid guid;

        Rhi::ShaderCompileDesc compileDesc{};

        switch (type)
        {
        case Type::EntityCollectPass_CS:
        {
            guid              = Guid::FromString(ShaderGuids::s_EntityCollectPass);
            compileDesc.Stage = Rhi::ShaderCompileStage::Compute;
            break;
        }

        case Type::TiledForward_PS:
        {
            guid              = Guid::FromString(ShaderGuids::s_TiledForwardPass);
            compileDesc.Stage = Rhi::ShaderCompileStage::Pixel;
            compileDesc.Flags |= Rhi::ShaderCompileFlags::LibraryShader;
            break;
        }

        default:
            std::unreachable();
        }

        return LoadAndCompile({}, executor, assetStorage, std::move(guid), std::move(compileDesc));
    }
} // namespace Ame::Gfx::Cache
