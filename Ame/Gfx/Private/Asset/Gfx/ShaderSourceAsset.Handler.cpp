#include <Asset/Gfx/ShaderSourceAsset.Handler.hpp>

namespace Ame::Asset::Gfx
{
    ShaderSourceAsset::Handler::Handler(
        ShaderCache& Cache) :
        m_Cache(Cache)
    {
    }

    bool ShaderSourceAsset::Handler::CanHandle(
        const Ptr<IAsset>& Asset)
    {
        auto ShaderSource = dynamic_cast<ShaderSourceAsset*>(Asset.get());
        if (ShaderSource == nullptr)
        {
            return false;
        }
        return &ShaderSource->m_Cache == &m_Cache;
    }

    Ptr<IAsset> ShaderSourceAsset::Handler::Load(
        std::istream&                  Stream,
        const Asset::DependencyReader& DepReader,
        const Handle&                  AssetGuid,
        String                         Path,
        const AssetMetaData&           LoaderData)
    {
        Stream.seekg(0, std::ios::end);
        auto FileSize = Stream.tellg();
        Stream.seekg(std::ios::beg);

        String ShaderCode(FileSize, '\0');
        Stream.read(ShaderCode.data(), FileSize);

        return std::make_shared<ShaderSourceAsset>(m_Cache, std::move(ShaderCode), AssetGuid, std::move(Path));
    }

    void ShaderSourceAsset::Handler::Save(
        std::iostream&     Stream,
        DependencyWriter&  DepWriter,
        const Ptr<IAsset>& Asset,
        AssetMetaData&     LoaderData)
    {
        auto Shader = dynamic_cast<ShaderSourceAsset*>(Asset.get());
        Stream.write(Shader->m_ShaderSource.data(), Shader->m_ShaderSource.size());
    }
} // namespace Ame::Asset::Gfx