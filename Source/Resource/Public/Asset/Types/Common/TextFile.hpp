#pragma once

#include <Asset/Asset.hpp>
#include <optional>

namespace Ame::Asset::Common
{
    // {22EA167D-E6C2-4A94-8E54-F8F38FACEC25}
    static constexpr UId IID_TextFileAsset{ 0x22ea167d, 0xe6c2, 0x4a94, { 0x8e, 0x54, 0xf8, 0xf3, 0x8f, 0xac, 0xec, 0x25 } };

    class TextFileAsset : public BaseObject<IAsset>
    {
    public:
        using Base = BaseObject<IAsset>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_TextFileAsset, IID_BaseSerializable, Base);

        TextFileAsset(
            IReferenceCounters* counters) :
            Base(counters)
        {
        }

    public:
        /// <summary>
        /// Get the underlying string
        /// </summary>
        [[nodiscard]] String& Get() noexcept;

        /// <summary>
        /// Get the underlying string
        /// </summary>
        [[nodiscard]] const String& Get() const noexcept;

        /// <summary>
        /// Get the underlying string
        /// </summary>
        void Set(
            String text);

    private:
        String m_Text;
    };
} // namespace Ame::Asset::Common