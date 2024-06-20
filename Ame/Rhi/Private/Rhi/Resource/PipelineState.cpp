#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    PipelineState::PipelineState(
        nri::CoreInterface&  nriCore,
        nri::PipelineLayout* layout,
        nri::Pipeline*       pipelineState) :
        m_NriCore(&nriCore),
        m_Layout(layout)
    {
    }

    //

    void PipelineState::SetName(
        const char* name) const
    {
        m_NriCore->SetPipelineDebugName(*m_Pipeline, name);
    }

    nri::Pipeline* const& PipelineState::Unwrap() const
    {
        return m_Pipeline;
    }

    //

    PipelineLayout PipelineState::GetLayout() const
    {
        return PipelineLayout(*m_NriCore, m_Layout);
    }

    //

    void ScopedPipelineLayout::Release()
    {
        m_Allocator->Release(*this).wait();
    }
} // namespace Ame::Rhi