#include <Rhi/Staging/StagedBuffer.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::Staging
{
    StagedBuffer::StagedBuffer(
        Device&           rhiDevice,
        StagedAccessType  accessType,
        const BufferDesc& desc) :
        m_Buffer(rhiDevice, StagedAccessToMemoryLocation(accessType), desc)
    {
    }

    const Buffer& StagedBuffer::GetBuffer() const
    {
        return m_Buffer;
    }

    size_t StagedBuffer::GetSize() const
    {
        return m_Buffer.GetDesc().size;
    }
} // namespace Ame::Rhi::Staging