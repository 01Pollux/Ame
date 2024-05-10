#pragma once

#include <Asset/Asset.hpp>

namespace Ame::Asset::Common
{
    class BinaryBufferAsset : public IAsset
    {
    public:
        static constexpr size_t UID = 1184421948659510;

        class Handler;

        BinaryBufferAsset(
            const Handle& AssetGuid,
            String        Path) :
            BinaryBufferAsset({}, AssetGuid, std::move(Path))
        {
        }

        BinaryBufferAsset(
            std::vector<uint8_t> Buffer,
            const Handle&        AssetGuid,
            String               Path) :
            IAsset(AssetGuid, std::move(Path)),
            m_Buffer(std::move(Buffer))
        {
        }

    public:
        /// <summary>
        /// Get data of the buffer
        /// </summary>
        [[nodiscard]] const uint8_t* GetData() const noexcept
        {
            return m_Buffer.data();
        }

        /// <summary>
        /// Get data of the buffer
        /// </summary>
        [[nodiscard]] uint8_t* GetData() noexcept
        {
            return m_Buffer.data();
        }

        /// <summary>
        /// Set data of the buffer
        /// </summary>
        void SetData(
            std::vector<uint8_t> Buffer)
        {
            m_Buffer = std::move(Buffer);
        }

        /// <summary>
        /// Get size of the buffer
        /// </summary>
        [[nodiscard]] size_t GetSize() const noexcept
        {
            return m_Buffer.size();
        }

    private:
        std::vector<uint8_t> m_Buffer;
    };
} // namespace Ame::Asset