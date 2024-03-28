#include <Rhi/View.hpp>

#include "NriError.hpp"

namespace Ame::Rhi
{
    void ResourceView::Release()
    {
    }

    void ResourceView::DeferRelease()
    {
    }

    //

    void ResourceView::SetName(
        const char* Name)
    {
    }

    nri::Descriptor* ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative() const
    {
        return nullptr;
    }
} // namespace Ame::Rhi