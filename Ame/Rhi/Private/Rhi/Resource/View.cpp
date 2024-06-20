#include <Rhi/Resource/View.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    ResourceView::ResourceView(
        nri::CoreInterface& nriCore,
        nri::Descriptor*    descriptor) :
        m_NriCore(&nriCore),
        m_Descriptor(descriptor)
    {
    }

    //

    void ResourceView::SetName(
        const char* name) const
    {
        m_NriCore->SetDescriptorDebugName(*m_Descriptor, name);
    }

    nri::Descriptor* const& ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative() const
    {
        return std::bit_cast<void*>(m_NriCore->GetDescriptorNativeObject(*m_Descriptor));
    }

    //

    void ScopedResourceView::Release()
    {
        m_Allocator->Release(*this).wait();
    }
} // namespace Ame::Rhi