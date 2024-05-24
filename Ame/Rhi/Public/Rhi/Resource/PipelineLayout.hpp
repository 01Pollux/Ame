#pragma once

#include <Rhi/Descs/Layout.hpp>

namespace Ame::Rhi
{
    class PipelineLayout final
    {
    public:
        PipelineLayout(
            Device&              RhiDevice,
            nri::PipelineLayout& Layout,
            size_t               Hash);

        PipelineLayout(const PipelineLayout&) = delete;
        PipelineLayout(PipelineLayout&&)      = delete;

        PipelineLayout& operator=(const PipelineLayout&) = delete;
        PipelineLayout& operator=(PipelineLayout&&)      = delete;

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

        /// <summary>
        /// Get the pipeline layout hash.
        /// </summary>
        [[nodiscard]] size_t GetHash() const;

    private:
        Device&              m_RhiDevice;
        nri::PipelineLayout& m_Layout;
        size_t               m_Hash = 0;
    };
} // namespace Ame::Rhi