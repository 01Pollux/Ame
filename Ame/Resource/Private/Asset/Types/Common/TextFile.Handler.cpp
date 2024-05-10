#include <Asset/Types/Common/TextFile.Handler.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace Ame::Asset::Common
{
    bool TextFileAsset::Handler::CanHandle(
        const Ptr<IAsset>& Resource)
    {
        return dynamic_cast<TextFileAsset*>(Resource.get());
    }

    Ptr<IAsset> TextFileAsset::Handler::Load(
        std::istream&                  Stream,
        const Asset::DependencyReader& DepReader,
        const Handle&                  AssetGuid,
        String                         Path,
        const AssetMetaData&           LoaderData)
    {
        std::stringstream Text;
        Text << Stream.rdbuf();
        return std::make_shared<TextFileAsset>(Text.str(), AssetGuid, std::move(Path));
    }

    void TextFileAsset::Handler::Save(
        std::iostream& Stream,
        DependencyWriter&,
        const Ptr<IAsset>& Asset,
        AssetMetaData&     LoaderData)
    {
        Stream << dynamic_cast<const TextFileAsset*>(Asset.get())->Get();
    }
} // namespace Ame::Asset
