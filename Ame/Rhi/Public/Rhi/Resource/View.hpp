#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/ScopedResource.hpp>

namespace Ame::Rhi
{
    class ResourceView
    {
    public:
        ResourceView() = default;
        ResourceView(std::nullptr_t)
        {
        }

        ResourceView(
            DeviceResourceAllocator& allocator,
            nri::Descriptor*         descriptor);

        ResourceView(const ResourceView&) = default;
        ResourceView(ResourceView&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_Descriptor(std::exchange(other.m_Descriptor, nullptr))
        {
        }

        ResourceView& operator=(const ResourceView&) = default;
        ResourceView& operator=(ResourceView&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator  = std::exchange(other.m_Allocator, nullptr);
                m_Descriptor = std::exchange(other.m_Descriptor, nullptr);
            }
            return *this;
        }

        ~ResourceView() = default;

    public:
        void Release(
            bool defer = true);

    public:
        [[nodiscard]] auto operator<=>(
            const ResourceView& other) const noexcept
        {
            return m_Descriptor <=> other.m_Descriptor;
        }

        [[nodiscard]] bool operator==(
            const ResourceView& other) const noexcept
        {
            return m_Descriptor == other.m_Descriptor;
        }

        explicit operator bool() const noexcept
        {
            return m_Descriptor != nullptr;
        }

    public:
        /// <summary>
        /// Set the resource view name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri descriptor.
        /// </summary>
        [[nodiscard]] nri::Descriptor* const& Unwrap() const;

        /// <summary>
        /// Get the descriptor native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

    protected:
        DeviceResourceAllocator* m_Allocator  = nullptr;
        nri::Descriptor*         m_Descriptor = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(ResourceView);
} // namespace Ame::Rhi