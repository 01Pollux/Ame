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
            DeviceResourceAllocator& allocator,
            nri::PipelineLayout*     layout,
            nri::Pipeline*           pipelineState);

        PipelineState(const PipelineState&) = default;
        PipelineState(PipelineState&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_Layout(std::exchange(other.m_Layout, nullptr)),
            m_Pipeline(std::exchange(other.m_Pipeline, nullptr))
        {
        }

        PipelineState& operator=(const PipelineState&) = default;
        PipelineState& operator=(PipelineState&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator = std::exchange(other.m_Allocator, nullptr);
                m_Layout    = std::exchange(other.m_Layout, nullptr);
                m_Pipeline  = std::exchange(other.m_Pipeline, nullptr);
            }
            return *this;
        }

        ~PipelineState() = default;

    public:
        void Release(
            bool defer = true);

    public:
        [[nodiscard]] auto operator<=>(
            const PipelineState& other) const noexcept
        {
            return m_Pipeline <=> other.m_Pipeline;
        }

        [[nodiscard]] bool operator==(
            const PipelineState& other) const noexcept
        {
            return m_Pipeline == other.m_Pipeline;
        }

        explicit operator bool() const noexcept
        {
            return m_Pipeline != nullptr;
        }

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
        DeviceResourceAllocator* m_Allocator = nullptr;
        nri::PipelineLayout*     m_Layout    = nullptr;
        nri::Pipeline*           m_Pipeline  = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(PipelineState);
} // namespace Ame::Rhi