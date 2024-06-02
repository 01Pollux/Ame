#pragma once

#include <Rhi/Staging/Desc.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi::Staging
{
    class StagedBuffer
    {
    public:
        StagedBuffer(
            Device&           rhiDevice,
            StagedAccessType  accessType,
            const BufferDesc& desc);

        StagedBuffer(
            Buffer buffer,
            size_t offset,
            size_t size);

    public:
        /// <summary>
        /// Get the buffer from the staged buffer.
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer() const;

        /// <summary>
        /// Get the size of the buffer.
        /// </summary>
        [[nodiscard]] size_t GetSize() const;

        /// <summary>
        /// Get the offset of the buffer.
        /// </summary>
        [[nodiscard]] size_t GetOffset() const;

        /// <summary>
        /// Get the pointer to the buffer.
        /// </summary>
        [[nodiscard]] const std::byte* GetPtr(
            size_t offset = 0) const;

        /// <summary>
        /// Get the pointer to the buffer.
        /// </summary>
        [[nodiscard]] std::byte* GetPtr(
            size_t offset = 0);

    private:
        Buffer m_Buffer;
        size_t m_Size   = 0;
        size_t m_Offset = 0;
    };
} // namespace Ame::Rhi::Staging