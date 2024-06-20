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
            nri::CoreInterface& nriCore,
            nri::Texture*       texture);

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
        nri::CoreInterface* m_NriCore = nullptr;
        nri::Texture*       m_Texture = nullptr;
    };

    //

    class ScopedTexture : public ScopedResource<ScopedTexture, Texture>
    {
        friend class ScopedResource;

    public:
        using ScopedResource::ScopedResource;

    protected:
        void Release();
    };
} // namespace Ame::Rhi