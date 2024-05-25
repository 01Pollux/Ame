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

    Ptr<IAsset> ShaderSourceAsset::Handler::Load(
        std::istream& stream,
        const Asset::DependencyReader&,
        const Guid& guid,
        String      path,
        const AssetMetaData&)
    {
        stream.seekg(0, std::ios::end);
        auto FileSize = stream.tellg();
        stream.seekg(std::ios::beg);

        String ShaderCode(FileSize, '\0');
        stream.read(ShaderCode.data(), FileSize);

        return std::make_shared<ShaderSourceAsset>(m_Cache, std::move(ShaderCode), guid, std::move(path));
    }

    void ShaderSourceAsset::Handler::Save(
        std::iostream& stream,
        DependencyWriter&,
        const Ptr<IAsset>& asset,
        AssetMetaData&)
    {
        auto assetShader = dynamic_cast<ShaderSourceAsset*>(asset.get());
        stream.write(assetShader->m_ShaderSource.data(), assetShader->m_ShaderSource.size());
    }
} // namespace Ame::Asset::Gfx