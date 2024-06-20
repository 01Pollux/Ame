#pragma once

#include <Rhi/Descs/Layout.hpp>
#include <Rhi/Resource/ScopedResource.hpp>

namespace Ame::Rhi
{
    class PipelineLayout
    {
    public:
        PipelineLayout() = default;
        PipelineLayout(nullptr_t)
        {
        }

        PipelineLayout(
            nri::CoreInterface&  nriCore,
            nri::PipelineLayout* layout);

    public:
        /// <summary>
        /// Set the pipeline layout name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri pipeline layout.
        /// </summary>
        [[nodiscard]] nri::PipelineLayout* const& Unwrap() const;

    private:
        nri::CoreInterface*  m_NriCore = nullptr;
        nri::PipelineLayout* m_Layout  = nullptr;
    };

    //

    class ScopedPipelineLayout : public ScopedResource<ScopedPipelineLayout, PipelineLayout>
    {
        friend class ScopedResource;

    public:
        using ScopedResource::ScopedResource;

    protected:
        void Release();
    };
} // namespace Ame::Rhi