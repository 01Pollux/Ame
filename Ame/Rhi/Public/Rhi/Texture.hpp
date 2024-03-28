#pragma once

#include <Rhi/Resource.hpp>
#include <Rhi/View.hpp>

namespace Ame::Rhi
{
    class Texture
    {
    public:
        Texture() = default;

        Texture(
            const TextureDesc& Desc);

        explicit Texture(
            nri::Texture* Tex) :
            m_Texture(Tex)
        {
        }

        operator bool() const noexcept
        {
            return m_Texture != nullptr;
        }

    public:
        /// <summary>
        /// Immediately releases the buffer.
        /// </summary>
        void Release();

        /// <summary>
        /// Defers the release of the buffer until the gpu is done with it.
        /// </summary>
        void DeferRelease();

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* Name);

        /// <summary>
        /// Get the buffer description.
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
        [[nodiscard]] ResourceView CreateShaderView(
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
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] ResourceView CreateView(
            const TextureViewDesc& Desc) const;

    private:
        nri::Texture* m_Texture = nullptr;
    };
} // namespace Ame::Rhi