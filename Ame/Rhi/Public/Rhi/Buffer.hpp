#pragma once

#include <Rhi/Resource.hpp>
#include <Rhi/View.hpp>

namespace Ame::Rhi
{
    class Buffer
    {
    public:
        Buffer() = default;

        Buffer(
            const BufferDesc& Desc);

        explicit Buffer(
            nri::Buffer* Buf) :
            m_Buffer(Buf)
        {
        }

        operator bool() const noexcept
        {
            return m_Buffer != nullptr;
        }

    public:
        /// <summary>
        /// Immediately releases the buffer.
        /// </summary>
        void Release();

        /// <summary>
        /// Defers the release of the buffer until the gpu is done with it.
        /// </summary>
        void DeferRelease();

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* Name);

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

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] BufferResourceView CreateView(
            const BufferViewDesc& Desc) const;

    private:
        nri::Buffer* m_Buffer = nullptr;
    };
} // namespace Ame::Rhi