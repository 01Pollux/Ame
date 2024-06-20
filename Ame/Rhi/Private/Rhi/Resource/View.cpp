#include <Rhi/Resource/View.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    ResourceView::ResourceView(
        DeviceResourceAllocator& allocator,
        nri::Descriptor*         descriptor) :
        m_Allocator(&allocator),
        m_Descriptor(descriptor)
    {
    }

    void ResourceView::Release(
        bool defer)
    {
        if (m_Descriptor)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_Descriptor);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyDescriptor(*m_Descriptor);
            }
            m_Descriptor = nullptr;
        }
    }

    //

    void ResourceView::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetDescriptorDebugName(*m_Descriptor, name);
    }

    nri::Descriptor* const& ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative() const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        return std::bit_cast<void*>(nriCore.GetDescriptorNativeObject(*m_Descriptor));
    }
} // namespace Ame::Rhi