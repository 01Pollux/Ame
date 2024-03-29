#include <Rhi/View.hpp>

#include "NriError.hpp"

namespace Ame::Rhi
{
    void ResourceView::Release(
        Device& RhiDevice)
    {
        RhiDevice.Release(*m_Descriptor, false);
        m_Descriptor = nullptr;
    }

    void ResourceView::DeferRelease(
        Device& RhiDevice)
    {
        RhiDevice.Release(*m_Descriptor, false);
        m_Descriptor = nullptr;
    }

    //

    void ResourceView::SetName(
        Device&     RhiDevice,
        const char* Name) const
    {
		RhiDevice.SetName(*m_Descriptor, Name);
    }

    nri::Descriptor* ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative(
        Device& RhiDevice) const
    {
        return RhiDevice.GetNative(*m_Descriptor);
    }
} // namespace Ame::Rhi