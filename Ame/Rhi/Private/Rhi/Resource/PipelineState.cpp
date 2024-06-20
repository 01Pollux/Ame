#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    PipelineState::PipelineState(
        DeviceResourceAllocator& allocator,
        nri::PipelineLayout*     layout,
        nri::Pipeline*           pipelineState) :
        m_Allocator(&allocator),
        m_Layout(layout),
        m_Pipeline(pipelineState)
    {
    }

    void PipelineState::Release(
        bool defer)
    {
        if (m_Pipeline)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_Pipeline);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyPipeline(*m_Pipeline);
            }
            m_Pipeline = nullptr;
        }
    }

    //

    void PipelineState::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetPipelineDebugName(*m_Pipeline, name);
    }

    nri::Pipeline* const& PipelineState::Unwrap() const
    {
        return m_Pipeline;
    }

    //

    PipelineLayout PipelineState::GetLayout() const
    {
        return PipelineLayout(*m_Allocator, m_Layout);
    }
} // namespace Ame::Rhi