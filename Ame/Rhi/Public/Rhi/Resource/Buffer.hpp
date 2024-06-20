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
            DeviceResourceAllocator& allocator,
            nri::Buffer*             buffer,
            MemoryLocation           location);

        Buffer(const Buffer&) = default;
        Buffer(Buffer&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_Buffer(std::exchange(other.m_Buffer, nullptr)),
            m_Mapped(std::exchange(other.m_Mapped, nullptr))
        {
        }

        Buffer& operator=(const Buffer&) = default;
        Buffer& operator=(Buffer&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator = std::exchange(other.m_Allocator, nullptr);
                m_Buffer    = std::exchange(other.m_Buffer, nullptr);
                m_Mapped    = std::exchange(other.m_Mapped, nullptr);
            }
            return *this;
        }

        ~Buffer() = default;

    public:
        void Release(
            bool defer = true);

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
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] ResourceView CreateView(
            const BufferViewDesc& desc) const;

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
        DeviceResourceAllocator* m_Allocator = nullptr;
        nri::Buffer*             m_Buffer    = nullptr;
        void*                    m_Mapped    = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(Buffer);
} // namespace Ame::Rhi