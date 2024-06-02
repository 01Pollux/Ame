#pragma once

#include <Asset/Asset.hpp>
#include <optional>

namespace Ame::Asset::Common
{
    class TextFileAsset : public IAsset
    {
    public:
        class Handler;

        using IAsset::IAsset;

        TextFileAsset(
            String      text,
            const Guid& guid,
            String      path);

        /// <summary>
        /// Get string
        /// </summary>
        [[nodiscard]] const String& Get() const;

        /// <summary>
        /// Get string
        /// </summary>
        void Set(
            const String& text);

    private:
        String m_Text;
    };
} // namespace Ame::Asset::Common