#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class Texture
    {
    public:
        struct Extern
        {
        };

        Texture() = default;
        Texture(std::nullptr_t) :
            m_Owning(false)
        {
        }

        Texture(
            Extern,
            DeviceImpl&   rhiDeviceImpl,
            nri::Texture* nriTexture);
        Texture(
            Extern,
            Device&       rhiDevice,
            nri::Texture* nriTexture);

        Texture(
            Device&            rhiDevice,
            MemoryLocation     location,
            const TextureDesc& desc);

    public:
        Texture(const Texture& other) = delete;
        Texture(Texture&& Other) noexcept;

        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept;

        ~Texture();

        auto operator==(
            const Texture& other) const
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
        [[nodiscard]] nri::Texture* Unwrap() const;

        /// <summary>
        /// Get the texture native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

        public:
        /// <summary>
        /// Borrow the texture (The texture will not be released when the texture is destroyed)
        /// </summary>
        [[nodiscard]] Texture Borrow() const;

        /// <summary>
        /// Check if the texture is owning. (If the texture is owning, it will be released when the texture is destroyed)
        /// </summary>
        [[nodiscard]] bool IsOwning() const;

    public:
        /// <summary>
        /// Create a texture view.
        /// </summary>
        [[nodiscard]] ResourceView CreateView(
            const TextureViewDesc& desc) const;

        /// <summary>
        /// Create a shader resource view.
        /// </summary>
        [[nodiscard]] ShaderResourceView CreateShaderView(
            const TextureViewDesc& desc) const;

        /// <summary>
        /// Create a render target view.
        /// </summary>
        [[nodiscard]] RenderTargetResourceView CreateRenderTargetView(
            const TextureViewDesc& desc) const;

        /// <summary>
        /// Create a depth stencil view.
        /// </summary>
        [[nodiscard]] DepthStencilResourceView CreateDepthStencilView(
            const TextureViewDesc& desc) const;

    private:
        /// <summary>
        /// Releases the texture.
        /// </summary>
        void Release();

    private:
        DeviceImpl*   m_Device  = nullptr;
        nri::Texture* m_Texture = nullptr;
        bool          m_Owning  = true;
    };
} // namespace Ame::Rhi