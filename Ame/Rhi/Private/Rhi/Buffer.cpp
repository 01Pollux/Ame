#include <Rhi/Buffer.hpp>

#include "NriError.hpp"

namespace Ame::Rhi
{
    Buffer::Buffer(
        const BufferDesc& Desc)
    {
    }

    //

    void Buffer::Release()
    {
    }

    void Buffer::DeferRelease()
    {
    }

    //

    void Buffer::SetName(
        const char* Name)
    {
    }

    const BufferDesc& Buffer::GetDesc() const
    {
        static BufferDesc desc;
        return desc;
    }

    nri::Buffer* Buffer::Unwrap() const
    {
        return nullptr;
    }

    void* Buffer::GetNative() const
    {
        return nullptr;
    }

    //

    BufferResourceView Buffer::CreateView(
        const BufferViewDesc& Desc) const
    {
        return BufferResourceView();
    }
} // namespace Ame::Rhi