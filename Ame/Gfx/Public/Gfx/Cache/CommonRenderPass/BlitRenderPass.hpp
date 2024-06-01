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

        uint32_t SrcMipLevel = 0;
        uint32_t DstMipLevel = 0;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;

        bool PlacePreBarrier  : 1 = true;
        bool PlacePostBarrier : 1 = false;
        bool FlushPostBarrier : 1 = false;

        Rhi::AccessLayoutStage NewState;
    };

    class BlitRenderPass
    {
        enum class OptimalBlitOperation : uint8_t
        {
            Copy,
            Render
        };

        struct BlitOperation
        {
            Rhi::CommandList      CommandList;
            const BlitParameters& Parameters;

            Rhi::TextureRect SrcRect;
            Rhi::TextureRect DstRect;

            std::vector<Rhi::TextureSubresource> SrcSubresources;
            std::vector<Rhi::TextureSubresource> DstSubresources;

            OptimalBlitOperation OptimalType;

            BlitOperation(
                Rhi::Device&          rhiDevice,
                const BlitParameters& parameters);
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

    private:
        [[nodiscard]] static OptimalBlitOperation QueryOptimalOperation(
            const BlitParameters& parameters);

        /// <summary>
        /// Resolve the subresources to be used in the blit operation.
        /// </summary>
        [[nodiscard]] static std::vector<Rhi::TextureSubresource> ResolveSubresources(
            const BlitParameters&                    parameters,
            std::span<const Rhi::TextureSubresource> subresources);

    private:
        /// <summary>
        /// Transition the textures to the correct state for a copy operation.
        /// </summary>
        void BlitCopyBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Blit a texture to another texture using a copy operation.
        /// </summary>
        void BlitCopy(
            BlitOperation& operation);

    private:
        /// <summary>
        /// Transition the textures to the correct state for a render operation.
        /// </summary>
        void BlitRenderBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Blit a texture to another texture using a render operation.
        /// </summary>
        void BlitRender(
            BlitOperation& operation);

    private:
        /// <summary>
        /// Transition the textures to the old state after a blit operation.
        /// </summary>
        void BlitRestoreState(
            BlitOperation& operation);

    private:
        Ref<Rhi::Device> m_Device;
    };
} // namespace Ame::Gfx::Cache