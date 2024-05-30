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

        /// <summary>
        /// Get the buffer from the staged buffer.
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer() const;

        /// <summary>
        /// Get the size of the buffer.
        /// </summary>
        [[nodiscard]] size_t GetSize() const;

    private:
        Buffer m_Buffer;
    };
} // namespace Ame::Rhi::Staging