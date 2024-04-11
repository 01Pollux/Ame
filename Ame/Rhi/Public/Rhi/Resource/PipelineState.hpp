#pragma once

#include <Rhi/Descs/Pipeline.hpp>

namespace Ame::Rhi
{
    class PipelineState final : public NonCopyable,
                                public NonMovable
    {
    public:
        PipelineState(
            Device&             RhiDevice,
            Ptr<PipelineLayout> Layout,
            nri::Pipeline&      Pipeline);

        ~PipelineState();

    public:
        /// <summary>
        /// Set the pipeline state name.
        /// </summary>
        void SetName(
            const char* Name) const;

        /// <summary>
        /// Get the nri pipeline state.
        /// </summary>
        [[nodiscard]] nri::Pipeline& Unwrap() const;

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