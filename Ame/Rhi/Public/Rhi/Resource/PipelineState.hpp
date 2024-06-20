#pragma once

#include <Rhi/Descs/Pipeline.hpp>
#include <Rhi/Resource/ScopedResource.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>

namespace Ame::Rhi
{
    class PipelineState
    {
    public:
        PipelineState() = default;
        PipelineState(nullptr_t)
        {
        }

        PipelineState(
            nri::CoreInterface&  nriCore,
            nri::PipelineLayout* layout,
            nri::Pipeline*       pipelineState);

    public:
        /// <summary>
        /// Set the pipeline state name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri pipeline state.
        /// </summary>
        [[nodiscard]] nri::Pipeline* const& Unwrap() const;

    public:
        /// <summary>
        /// Get the pipeline layout.
        /// </summary>
        [[nodiscard]] PipelineLayout GetLayout() const;

    private:
        nri::CoreInterface*  m_NriCore  = nullptr;
        nri::PipelineLayout* m_Layout   = nullptr;
        nri::Pipeline*       m_Pipeline = nullptr;
    };
    //

    class ScopedPipelineState : public ScopedResource<ScopedPipelineState, PipelineState>
    {
        friend class ScopedResource;

    public:
        using ScopedResource::ScopedResource;

    protected:
        void Release();
    };
} // namespace Ame::Rhi