#pragma once

#include <Rhi/Resource.hpp>

namespace Ame::Rhi
{
    class PipelineLayout final : public NonCopyable,
                                 public NonMovable
    {
    public:
        PipelineLayout(
            Device&              RhiDevice,
            nri::PipelineLayout& Layout);

        ~PipelineLayout();

    public:
        /// <summary>
        /// Set the pipeline layout name.
        /// </summary>
        void SetName(
            const char* Name) const;

        /// <summary>
        /// Get the nri pipeline layout.
        /// </summary>
        [[nodiscard]] nri::PipelineLayout& Unwrap() const;

    private:
        Device&              m_RhiDevice;
        nri::PipelineLayout& m_Layout;
    };
} // namespace Ame::Rhi