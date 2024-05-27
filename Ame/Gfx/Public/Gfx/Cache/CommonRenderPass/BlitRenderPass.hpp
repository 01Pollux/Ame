#pragma once

#include <Rhi/Resource/Texture.hpp>

namespace Ame::Gfx::Cache
{
    struct BlitParameters
    {
        Ref<const Rhi::Texture> SrcTexture;
        Ref<const Rhi::Texture> DstTexture;

        Rhi::TextureRect SrcRect = Rhi::c_EntireTexture;
        Rhi::TextureRect DstRect = Rhi::c_EntireTexture;

        std::span<const Rhi::TextureSubresource> SrcSubresources;
        std::span<const Rhi::TextureSubresource> DstSubresources;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;
    };

    struct SingleBlitParameters
    {
        Ref<const Rhi::Texture> SrcTexture;
        Ref<const Rhi::Texture> DstTexture;

        Rhi::TextureRect SrcRect = Rhi::c_EntireTexture;
        Rhi::TextureRect DstRect = Rhi::c_EntireTexture;

        Rhi::TextureSubresource SrcSubresources = Rhi::c_AllSubresources;
        Rhi::TextureSubresource DstSubresources = Rhi::c_AllSubresources;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;
    };

    class BlitRenderPass
    {
        enum class OptimalBlitOperation : uint8_t
        {
            Copy,
            Render
        };

    public:
        BlitRenderPass(
            Rhi::Device& rhiDevice);

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
        [[nodiscard]] OptimalBlitOperation QueryOptimalOperation(
            const BlitParameters& parameters);

        /// <summary>
        /// Blit a texture to another texture using a copy operation.
        /// </summary>
        void BlitCopy(
            Rhi::CommandList&     commandList,
            const BlitParameters& parameters);

        /// <summary>
        /// Blit a texture to another texture using a render operation.
        /// </summary>
        void BlitRender(
            Rhi::CommandList&     commandList,
            const BlitParameters& parameters);

    private:
        Ref<Rhi::Device> m_Device;
    };
} // namespace Ame::Gfx::Cache