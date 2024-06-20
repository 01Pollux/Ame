#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Staging/Desc.hpp>

namespace Ame::Rhi::Staging
{
    class StagedBuffer : public Buffer
    {
    public:
        [[nodiscard]] static StagedBuffer Create(
            DeviceResourceAllocator& allocator,
            StagedAccessType         accessType,
            const BufferDesc&        desc);

        [[nodiscard]] static StagedBuffer CreateForView(
            Buffer buffer,
            size_t offset,
            size_t size);

    public:
        StagedBuffer() = default;
        StagedBuffer(std::nullptr_t)
        {
        }

    protected:
        StagedBuffer(
            Buffer buffer,
            size_t offset,
            size_t size);

    public:
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
        size_t m_Offset = 0;
        size_t m_Size   = 0;
    };

    AME_RHI_SCOPED_RESOURCE(StagedBuffer);
} // namespace Ame::Rhi::Staging