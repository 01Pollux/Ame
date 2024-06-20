#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/Core.hpp>

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
            DeviceResourceAllocator& allocator,
            BaseTy&&                 resource) :
            BaseTy(std::move(resource)),
            m_Allocator(&allocator)
        {
        }

        ScopedResource(const ScopedResource&) = delete;
        ScopedResource(ScopedResource&& other) noexcept :
            BaseTy(std::move(other)),
            m_Allocator(std::move(other.m_Allocator))
        {
        }

        ScopedResource& operator=(const ScopedResource&) = delete;
        ScopedResource& operator=(ScopedResource&& other) noexcept
        {
            if (this != &other)
            {
                static_cast<Ty*>(this)->Release();
                m_Allocator = std::exchange(other.m_Allocator, nullptr);
                BaseTy::operator=(std::move(other));
            }
            return *this;
        }

        ~ScopedResource()
        {
            static_cast<Ty*>(this)->Release();
        }

    protected:
        DeviceResourceAllocator* m_Allocator;
    };
} // namespace Ame::Rhi