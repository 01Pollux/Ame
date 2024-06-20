#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/ScopedResource.hpp>

namespace Ame::Rhi
{
    class Buffer
    {
    public:
        Buffer() = default;
        Buffer(std::nullptr_t)
        {
        }

        Buffer(
            nri::CoreInterface& nriCore,
            nri::Buffer*        buffer,
            MemoryLocation      location);

    public:
        [[nodiscard]] auto operator<=>(
            const Buffer& other) const noexcept
        {
            return m_Buffer <=> other.m_Buffer;
        }

        [[nodiscard]] bool operator==(
            const Buffer& other) const noexcept
        {
            return m_Buffer == other.m_Buffer;
        }

        explicit operator bool() const noexcept
        {
            return m_Buffer != nullptr;
        }

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the buffer description.
        /// </summary>
        [[nodiscard]] const BufferDesc& GetDesc() const;

        /// <summary>
        /// Get the nri buffer.
        /// </summary>
        [[nodiscard]] nri::Buffer* const& Unwrap() const;

        /// <summary>
        /// Get the buffer native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

    public:
        /// <summary>
        /// Get the buffer pointer. (Only for host visible buffers)
        /// </summary>
        [[nodiscard]] std::byte* GetPtr(
            size_t offset = 0);

        /// <summary>
        /// Get the buffer pointer. (Only for host visible buffers)
        /// </summary>
        [[nodiscard]] const std::byte* GetPtr(
            size_t offset = 0) const;

    private:
        nri::CoreInterface* m_NriCore = nullptr;
        nri::Buffer*        m_Buffer  = nullptr;
        void*               m_Mapped  = nullptr;
    };

    class ScopedBuffer : public ScopedResource<ScopedBuffer, Buffer>
    {
        friend class ScopedResource;

    public:
        using ScopedResource::ScopedResource;

    private:
        void Release();
    };
} // namespace Ame::Rhi