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
            DeviceResourceAllocator& allocator,
            nri::PipelineLayout*     layout);

        PipelineLayout(const PipelineLayout&) = default;
        PipelineLayout(PipelineLayout&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_Layout(std::exchange(other.m_Layout, nullptr))
        {
        }

        PipelineLayout& operator=(const PipelineLayout&) = default;
        PipelineLayout& operator=(PipelineLayout&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator = std::exchange(other.m_Allocator, nullptr);
                m_Layout    = std::exchange(other.m_Layout, nullptr);
            }
            return *this;
        }

        ~PipelineLayout() = default;

    public:
        void Release(
            bool defer = true);

    public:
        [[nodiscard]] auto operator<=>(
            const PipelineLayout& other) const noexcept
        {
            return m_Layout <=> other.m_Layout;
        }

        [[nodiscard]] bool operator==(
            const PipelineLayout& other) const noexcept
        {
            return m_Layout == other.m_Layout;
        }

        explicit operator bool() const noexcept
        {
            return m_Layout != nullptr;
        }

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
        DeviceResourceAllocator* m_Allocator = nullptr;
        nri::PipelineLayout*     m_Layout    = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(PipelineLayout);
} // namespace Ame::Rhi