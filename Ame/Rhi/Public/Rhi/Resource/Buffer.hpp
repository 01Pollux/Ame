#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class Buffer
    {
        friend class Device;

    protected:
        Buffer(
            Device&        rhiDevice,
            MemoryLocation location,
            nri::Buffer*   nriBuffer);

    public:
        struct Extern
        {
        };

        Buffer() = default;
        Buffer(std::nullptr_t) :
            m_Owning(false)
        {
        }

        Buffer(
            Extern,
            DeviceImpl&  rhiDeviceImpl,
            nri::Buffer* nriBuffer);
        Buffer(
            Extern,
            Device&      rhiDevice,
            nri::Buffer* nriBuffer);

        Buffer(
            Device&           rhiDevice,
            MemoryLocation    location,
            const BufferDesc& desc);

    public:
        Buffer(const Buffer&);
        Buffer(Buffer&& other) noexcept;

        Buffer& operator=(const Buffer&);
        Buffer& operator=(Buffer&& other) noexcept;

        ~Buffer();

        operator bool() const noexcept
        {
            return m_Buffer != nullptr;
        }

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* name);

        /// <summary>
        /// Get the buffer description.
        /// </summary>
        [[nodiscard]] const BufferDesc& GetDesc() const;

        /// <summary>
        /// Get the nri buffer.
        /// </summary>
        [[nodiscard]] nri::Buffer* Unwrap() const;

        /// <summary>
        /// Get the buffer native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

        /// <summary>
        /// Check if the buffer is owning. (If the buffer is owning, it will be released when the buffer is destroyed)
        /// </summary>
        [[nodiscard]] bool IsOwning() const;

    public:
        /// <summary>
        /// Get the buffer pointer. (Only for host visible buffers)
        /// </summary>
        void* GetPtr(
            size_t offset = 0);

        /// <summary>
        /// Get the buffer pointer. (Only for host visible buffers)
        /// </summary>
        const void* GetPtr(
            size_t offset = 0) const;

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] BufferResourceView CreateView(
            const BufferViewDesc& desc) const;

    private:
        /// <summary>
        /// Releases the buffer.
        /// </summary>
        void Release();

    private:
        DeviceImpl*  m_Device = nullptr;
        nri::Buffer* m_Buffer = nullptr;
        void*        m_Mapped = nullptr;
        bool         m_Owning = true;
    };
} // namespace Ame::Rhi