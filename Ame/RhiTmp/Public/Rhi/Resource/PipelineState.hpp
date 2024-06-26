#pragma once

#include <Rhi/Descs/Pipeline.hpp>

namespace Ame::Rhi
{
    class PipelineState final
    {
    public:
        PipelineState(
            Device&             rhiDevice,
            Ptr<PipelineLayout> layout,
            nri::Pipeline&      nriPipeline);

        PipelineState(const PipelineState&) = delete;
        PipelineState(PipelineState&&)      = delete;

        PipelineState& operator=(const PipelineState&) = delete;
        PipelineState& operator=(PipelineState&&)      = delete;

        ~PipelineState();

    public:
        /// <summary>
        /// Set the pipeline state name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri pipeline state.
        /// </summary>
        [[nodiscard]] const nri::Pipeline& Unwrap() const;

        /// <summary>
        /// Get the pipeline layout.
        /// </summary>
        [[nodiscard]] Ptr<PipelineLayout> GetLayout() const;

    private:
        Device&             m_RhiDevice;
        Ptr<PipelineLayout> m_Layout;
        nri::Pipeline&      m_Pipeline;
    };
} // namespace Ame::Rhi