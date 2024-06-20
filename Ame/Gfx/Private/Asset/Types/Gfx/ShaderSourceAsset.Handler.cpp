#include <Asset/Types/Gfx/ShaderSourceAsset.Handler.hpp>

namespace Ame::Asset::Gfx
{
    ShaderSourceAsset::Handler::Handler(
        ShaderCache& shaderCache) :
        m_Cache(shaderCache)
    {
    }

    bool ShaderSourceAsset::Handler::CanHandle(
        const Ptr<IAsset>& asset)
    {
        auto shaderAsset = dynamic_cast<ShaderSourceAsset*>(asset.get());
        if (shaderAsset == nullptr)
        {
            return false;
        }
        return &shaderAsset->m_Cache == &m_Cache;
    }

    Co::result<Ptr<IAsset>> ShaderSourceAsset::Handler::Load(
        AssetHandlerLoadDesc& loadDesc)
    {
        auto shaderCode =
            loadDesc.BackgroundExecutor
                ->submit(
                    [&]
                    {
                        std::stringstream stream;
                        stream << loadDesc.Stream.get().rdbuf();
                        return stream.str();
                    })
                .get();
        co_return std::make_shared<ShaderSourceAsset>(m_Cache, std::move(shaderCode), loadDesc.Guid, std::move(loadDesc.Path));
    }

    Co::result<void> ShaderSourceAsset::Handler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        auto assetShader = dynamic_cast<ShaderSourceAsset*>(saveDesc.Asset.get());
        saveDesc.BackgroundExecutor
            ->submit(
                [&]
                {
                    saveDesc.Stream.get().write(assetShader->m_ShaderSource.data(), assetShader->m_ShaderSource.size());
                })
            .wait();
        co_return;
    }
} // namespace Ame::Asset::Gfx