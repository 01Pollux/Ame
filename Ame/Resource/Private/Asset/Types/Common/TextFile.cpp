#include <Asset/Types/Common/TextFile.hpp>

namespace Ame::Asset::Common
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
} // namespace Ame::Asset
