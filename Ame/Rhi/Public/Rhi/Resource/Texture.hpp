#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/ScopedResource.hpp>

namespace Ame::Rhi
{
    class Texture
    {
    public:
        Texture() = default;
        Texture(std::nullptr_t)
        {
        }

        Texture(
            DeviceResourceAllocator& allocator,
            nri::Texture*            texture);

        Texture(const Texture&) = default;
        Texture(Texture&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_Texture(std::exchange(other.m_Texture, nullptr))
        {
        }

        Texture& operator=(const Texture&) = default;
        Texture& operator=(Texture&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator = std::exchange(other.m_Allocator, nullptr);
                m_Texture   = std::exchange(other.m_Texture, nullptr);
            }
            return *this;
        }

        ~Texture() = default;

    public:
        void Release(
            bool defer = true);

    public:
        [[nodiscard]] auto operator<=>(
            const Texture& other) const noexcept
        {
            return m_Texture <=> other.m_Texture;
        }

        [[nodiscard]] bool operator==(
            const Texture& other) const noexcept
        {
            return m_Texture == other.m_Texture;
        }

        explicit operator bool() const noexcept
        {
            return m_Texture != nullptr;
        }

    public:
        /// <summary>
        /// Create a texture view.
        /// </summary>
        [[nodiscard]] ResourceView CreateView(
            const TextureViewDesc& viewDesc) const;

    public:
        /// <summary>
        /// Set the texture name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the texture description.
        /// </summary>
        [[nodiscard]] const TextureDesc& GetDesc() const;

        /// <summary>
        /// Get the nri texture.
        /// </summary>
        [[nodiscard]] nri::Texture* const& Unwrap() const;

        /// <summary>
        /// Get the texture native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

    private:
        DeviceResourceAllocator* m_Allocator = nullptr;
        nri::Texture*            m_Texture   = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(Texture);
} // namespace Ame::Rhi