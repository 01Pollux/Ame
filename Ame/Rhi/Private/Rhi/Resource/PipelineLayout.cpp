#include <Rhi/Resource/PipelineLayout.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    PipelineLayout::PipelineLayout(
        DeviceResourceAllocator& allocator,
        nri::PipelineLayout*     layout) :
        m_Allocator(&allocator),
        m_Layout(layout)
    {
    }

    void PipelineLayout::Release(
        bool defer)
    {
        if (m_Layout)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_Layout);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyPipelineLayout(*m_Layout);
            }
            m_Layout = nullptr;
        }
    }

    //

    void PipelineLayout::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetPipelineLayoutDebugName(*m_Layout, name);
    }

    nri::PipelineLayout* const& PipelineLayout::Unwrap() const
    {
        return m_Layout;
    }
} // namespace Ame::Rhi