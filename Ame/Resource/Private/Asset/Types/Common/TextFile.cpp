#include <Asset/Types/Common/TextFile.hpp>

namespace Ame::Asset::Common
{
    TextFileAsset::TextFileAsset(
        String      text,
        const Guid& guid,
        String      path) :
        IAsset(guid, std::move(path)),
        m_Text(std::move(text))
    {
    }

    const String& TextFileAsset::Get() const
    {
        return m_Text;
    }

    void TextFileAsset::Set(
        const String& text)
    {
        MarkDirty();
        m_Text = text;
    }
} // namespace Ame::Asset::Common
