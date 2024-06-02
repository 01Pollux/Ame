#pragma once

#include <Asset/Asset.hpp>

namespace Ame::Asset::Common
{
    class BinaryBufferAsset : public IAsset
    {
    public:
        class Handler;

        BinaryBufferAsset(
            const Guid& guid,
            String      path) :
            BinaryBufferAsset({}, guid, std::move(path))
        {
        }

        BinaryBufferAsset(
            std::vector<std::byte> buffer,
            const Guid&            guid,
            String                 path) :
            IAsset(guid, std::move(path)),
            m_Buffer(std::move(buffer))
        {
        }

    public:
        /// <summary>
        /// Get data of the buffer
        /// </summary>
        [[nodiscard]] const std::byte* GetData() const noexcept
        {
            return m_Buffer.data();
        }

        /// <summary>
        /// Get data of the buffer
        /// </summary>
        [[nodiscard]] std::byte* GetData() noexcept
        {
            return m_Buffer.data();
        }

        /// <summary>
        /// Set data of the buffer
        /// </summary>
        void SetData(
            std::vector<std::byte> buffer)
        {
            m_Buffer = std::move(buffer);
        }

        /// <summary>
        /// Get size of the buffer
        /// </summary>
        [[nodiscard]] size_t GetSize() const noexcept
        {
            return m_Buffer.size();
        }

    private:
        std::vector<std::byte> m_Buffer;
    };
} // namespace Ame::Asset::Common