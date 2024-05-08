#include <Asset/Handlers/TextFile.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace Ame::Asset
{
    TextFileAsset::TextFileAsset(
        String        Text,
        const Handle& AssetGuid,
        String        Path) :
        IAsset(AssetGuid, std::move(Path)),
        m_Text(std::move(Text))
    {
    }

    const String& TextFileAsset::Get() const
    {
        return m_Text;
    }

    void TextFileAsset::Set(
        const String& Text)
    {
        MarkDirty();
        m_Text = Text;
    }

    //

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
