#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class Texture
    {
    public:
        Texture(
            Device&            RhiDevice,
            MemoryLocation     Location,
            const TextureDesc& Desc);

        explicit Texture(
            nri::Texture* Tex = nullptr) :
            m_Texture(Tex)
        {
        }

        operator bool() const noexcept
        {
            return m_Texture != nullptr;
        }

    public:
        /// <summary>
        /// Immediately releases the texture.
        /// </summary>
        void Release(
            Device& RhiDevice);

        /// <summary>
        /// Defers the release of the texture until the gpu is done with it.
        /// </summary>
        void DeferRelease(
            Device& RhiDevice);

    public:
        /// <summary>
        /// Set the texture name.
        /// </summary>
        void SetName(
            Device&     RhiDevice,
            const char* Name) const;

        /// <summary>
        /// Get the texture description.
        /// </summary>
        [[nodiscard]] const TextureDesc& GetDesc(
            Device& RhiDevice) const;

        /// <summary>
        /// Get the nri texture.
        /// </summary>
        [[nodiscard]] nri::Texture* Unwrap() const;

        /// <summary>
        /// Get the texture native handle.
        /// </summary>
        [[nodiscard]] void* GetNative(
            Device& RhiDevice) const;

    public:
        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] ResourceView CreateShaderView(
            Device&                RhiDevice,
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] RenderTargetResourceView CreateRenderTargetView(
            Device&                RhiDevice,
            const TextureViewDesc& Desc) const;

        /// <summary>
        /// Create a buffer view.
        /// </summary>
        [[nodiscard]] DepthStencilResourceView CreateDepthStencilView(
            Device&                RhiDevice,
            const TextureViewDesc& Desc) const;

    private:
        nri::Texture* m_Texture = nullptr;
    };
} // namespace Ame::Rhi