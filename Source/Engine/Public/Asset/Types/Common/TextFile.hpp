#pragma once

#include <Asset/Asset.hpp>
#include <optional>

namespace Ame::Asset::Common
{
    class TextFileAsset : public IAsset
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_TextFileAsset, IAsset);

    private:
        IMPLEMENT_INTERFACE_CTOR(TextFileAsset) : IAsset(counters)
        {
        }

    public:
        void Serialize(BinaryOArchiver& ar) const override
        {
            ar(m_Text);
        }

        void Deserialize(BinaryIArchiver& ar) override
        {
            ar(m_Text);
        }

    public:
        /// <summary>
        /// Get the underlying string
        /// </summary>
        [[nodiscard]] String& GetRef() noexcept
        {
            return m_Text;
        }

        /// <summary>
        /// Get the underlying string
        /// </summary>
        [[nodiscard]] const String& Get() const noexcept
        {
            return m_Text;
        }

        /// <summary>
        /// Get the underlying string
        /// </summary>
        void Set(String text)
        {
            m_Text = std::move(text);
        }

    private:
        String m_Text;
    };
} // namespace Ame::Asset::Common