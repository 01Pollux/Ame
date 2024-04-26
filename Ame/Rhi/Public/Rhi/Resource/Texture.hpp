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
            DeviceImpl&   RhiDevice,
            nri::Texture* RhiTexture);
        Texture(
            Extern,
            Device&       RhiDevice,
            nri::Texture* RhiTexture);

        Texture(
            Device&            RhiDevice,
            MemoryLocation     Location,
            const TextureDesc& Desc);

    public:
        Texture(const Texture& Other);
        Texture(Texture&& Other) noexcept;

        Texture& operator=(const Texture& Other);
        Texture& operator=(Texture&& Other) noexcept;

        ~Texture();

        operator bool() const noexcept
        {
            return m_Texture != nullptr;
        }

    public:
        /// <summary>
        /// Set the texture name.
        /// </summary>
        void SetName(
            const char* Name) const;

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

        /// <summary>
        /// Check if the texture is owning. (If the texture is owning, it will be released when the texture is destroyed)
        /// </summary>
        [[nodiscard]] bool IsOwning() const;

    public:
        /// <summary>
        /// Create a texture view.
        /// </summary>
        [[nodiscard]] ResourceView CreateView(
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a shader resource view.
        /// </summary>
        [[nodiscard]] ShaderResourceView CreateShaderView(
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a render target view.
        /// </summary>
        [[nodiscard]] RenderTargetResourceView CreateRenderTargetView(
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a depth stencil view.
        /// </summary>
        [[nodiscard]] DepthStencilResourceView CreateDepthStencilView(
            const TextureViewDesc& Desc) const;

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