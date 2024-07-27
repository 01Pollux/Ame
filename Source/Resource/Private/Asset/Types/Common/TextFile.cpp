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

    String& TextFileAsset::Get() noexcept
    {
        return m_Text;
    }

    const String& TextFileAsset::Get() const noexcept
    {
        return m_Text;
    }

    void TextFileAsset::Set(
        String text)
    {
        MarkDirty();
        m_Text = std::move(text);
    }
} // namespace Ame::Asset::Common
