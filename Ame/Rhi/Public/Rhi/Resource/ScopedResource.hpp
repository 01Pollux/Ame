#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/Core.hpp>

#define AME_RHI_SCOPED_RESOURCE(Ty)                          \
    class Ty;                                                \
    class Scoped##Ty : public ScopedResource<Scoped##Ty, Ty> \
    {                                                        \
        friend class ScopedResource;                         \
                                                             \
    public:                                                  \
        using ScopedResource::ScopedResource;                \
    }

namespace Ame::Rhi
{
    template<typename Ty, typename BaseTy>
    class ScopedResource : public BaseTy
    {
    public:
        ScopedResource() = default;
        ScopedResource(std::nullptr_t) :
            BaseTy(nullptr)
        {
        }

        ScopedResource(
            BaseTy&& resource,
            bool     defer = true) :
            BaseTy(std::move(resource)),
            m_Defer(defer)
        {
        }

        ScopedResource(const ScopedResource&) = delete;
        ScopedResource(ScopedResource&& other) noexcept :
            BaseTy(std::move(other)),
            m_Defer(other.m_Defer)
        {
        }

        ScopedResource& operator=(const ScopedResource&) = delete;
        ScopedResource& operator=(ScopedResource&& other) noexcept
        {
            if (this != &other)
            {
                BaseTy::Release(m_Defer);

                m_Defer = other.m_Defer;
                BaseTy::operator=(std::move(other));
            }
            return *this;
        }

        ~ScopedResource()
        {
            BaseTy::Release(m_Defer);
        }

    public:
        void SetDeferred(
            bool defer = true)
        {
            m_Defer = defer;
        }

    protected:
        bool m_Defer = true;
    };
} // namespace Ame::Rhi