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
            nri::CoreInterface& nriCore,
            nri::Descriptor*    descriptor);

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
        nri::CoreInterface* m_NriCore    = nullptr;
        nri::Descriptor*    m_Descriptor = nullptr;
    };

    //

    class ScopedResourceView : public ScopedResource<ScopedResourceView, ResourceView>
    {
        friend class ScopedResource;

    public:
        using ScopedResource::ScopedResource;

    protected:
        void Release();
    };
} // namespace Ame::Rhi