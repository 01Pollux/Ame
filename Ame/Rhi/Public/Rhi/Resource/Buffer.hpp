#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class Buffer
    {
        friend class Device;

        Buffer(
            Device&        RhiDevice,
            MemoryLocation Location,
            nri::Buffer*   Buf);

    public:
        Buffer() = default;

        Buffer(
            Device&           RhiDevice,
            MemoryLocation    Location,
            const BufferDesc& Desc);

        operator bool() const noexcept
        {
            return m_Buffer != nullptr;
        }

    public:
        /// <summary>
        /// Releases the buffer.
        /// </summary>
        void Release(
            Device& RhiDevice,
            bool    Defer = true);

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            Device&     RhiDevice,
            const char* Name);

        /// <summary>
        /// Get the buffer description.
        /// </summary>
        [[nodiscard]] const BufferDesc& GetDesc(
            Device& RhiDevice) const;

        /// <summary>
        /// Get the nri buffer.
        /// </summary>
        [[nodiscard]] nri::Buffer* Unwrap(
            Device& RhiDevice) const;

        /// <summary>
        /// Get the buffer native handle.
        /// </summary>
        [[nodiscard]] void* GetNative(
            Device& RhiDevice) const;

    public:
        /// <summary>
        /// Get the buffer pointer. (Only for host visible buffers)
        /// </summary>
        void* GetPtr(
            size_t  Offset = 0);

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] BufferResourceView CreateView(
            Device&               RhiDevice,
            const BufferViewDesc& Desc) const;

    private:
        nri::Buffer* m_Buffer = nullptr;
        void*        m_Mapped = nullptr;
    };
} // namespace Ame::Rhi