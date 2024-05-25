#include <Asset/Types/Common/TextFile.Handler.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace Ame::Asset::Common
{
    bool TextFileAsset::Handler::CanHandle(
        const Ptr<IAsset>& asset)
    {
        return dynamic_cast<TextFileAsset*>(asset.get());
    }

    Ptr<IAsset> TextFileAsset::Handler::Load(
        std::istream& stream,
        const Asset::DependencyReader&,
        const Guid& guid,
        String      path,
        const AssetMetaData&)
    {
        std::stringstream text;
        text << stream.rdbuf();
        return std::make_shared<TextFileAsset>(text.str(), guid, std::move(path));
    }

    void TextFileAsset::Handler::Save(
        std::iostream& stream,
        DependencyWriter&,
        const Ptr<IAsset>& asset,
        AssetMetaData&)
    {
        stream << dynamic_cast<const TextFileAsset*>(asset.get())->Get();
    }
} // namespace Ame::Asset::Common
