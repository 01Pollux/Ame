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
        Texture(
            Extern,
            DeviceImpl&   RhiDevice,
            nri::Texture* RhiTexture);
        Texture(
            Extern,
            Device&       RhiDevice,
            nri::Texture* RhiTexture);
        Texture(std::nullptr_t) :
            m_Owning(false)
        {
        }

        Texture(
            Device&            RhiDevice,
            MemoryLocation     Location,
            const TextureDesc& Desc);

        Texture(const Texture&) = delete;
        Texture(Texture&& Other) noexcept;

        Texture& operator=(const Texture&) = delete;
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

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] ShaderResourceView CreateShaderView(
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] RenderTargetResourceView CreateRenderTargetView(
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a buffer view.
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