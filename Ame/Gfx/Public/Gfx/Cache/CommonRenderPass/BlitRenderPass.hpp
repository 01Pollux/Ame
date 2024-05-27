#pragma once

#include <Rhi/Resource/Texture.hpp>

namespace Ame::Gfx::Cache
{
    struct BlitParameters
    {
        Ref<Rhi::Texture> SrcTexture;
        Ref<Rhi::Texture> DstTexture;

        Rhi::TextureRect SrcRect = Rhi::c_EntireTexture;
        Rhi::TextureRect DstRect = Rhi::c_EntireTexture;

        std::span<Rhi::TextureSubresource> SrcSubresources;
        std::span<Rhi::TextureSubresource> DstSubresources;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;
    };

    struct SingleBlitParameters
    {
        Ref<Rhi::Texture> SrcTexture;
        Ref<Rhi::Texture> DstTexture;

        Rhi::TextureRect SrcRect = Rhi::c_EntireTexture;
        Rhi::TextureRect DstRect = Rhi::c_EntireTexture;

        Rhi::TextureSubresource SrcSubresources = Rhi::c_AllSubresources;
        Rhi::TextureSubresource DstSubresources = Rhi::c_AllSubresources;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;
    };

    class BlitRenderPass
    {
    public:
        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        void Blit(
            const BlitParameters& parameters);

        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        void Blit(
            const SingleBlitParameters& parameters);

    private:
    };
} // namespace Ame::Gfx::Cache