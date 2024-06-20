#include <Rhi/Resource/PipelineLayout.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    PipelineLayout::PipelineLayout(
        nri::CoreInterface&  nriCore,
        nri::PipelineLayout* layout) :
        m_NriCore(&nriCore),
        m_Layout(layout)
    {
    }

    //

    void PipelineLayout::SetName(
        const char* name) const
    {
        m_NriCore->SetPipelineLayoutDebugName(*m_Layout, name);
    }

    nri::PipelineLayout* const& PipelineLayout::Unwrap() const
    {
        return m_Layout;
    }

    //

    void ScopedPipelineLayout::Release()
    {
        m_Allocator->Release(*this).wait();
    }
} // namespace Ame::Rhi