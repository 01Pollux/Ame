#pragma once

#include <Asset/Asset.hpp>
#include <optional>

namespace Ame::Asset::Common
{
    class TextFileAsset : public IAsset
    {
    public:
        static constexpr size_t UID = 131732813173286877;

        class Handler;

        using IAsset::IAsset;

        TextFileAsset(
            String      Text,
            const Handle& AssetGuid,
            String      Path);

        /// <summary>
        /// Get string
        /// </summary>
        [[nodiscard]] const String& Get() const;

        /// <summary>
        /// Get string
        /// </summary>
        void Set(
            const String& Text);

    private:
        String m_Text;
    };
} // namespace Ame::Asset