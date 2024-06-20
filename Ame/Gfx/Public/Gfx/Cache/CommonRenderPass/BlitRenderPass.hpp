#pragma once

#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Descs/View.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Gfx::Cache
{
    struct BlitParameters
    {
        CRef<Rhi::Texture> SrcTexture;
        CRef<Rhi::Texture> DstTexture;

        Rhi::TextureRect SrcRect = Rhi::c_EntireTexture;
        Rhi::TextureRect DstRect = Rhi::c_EntireTexture;

        Rhi::AccessLayoutStage OldSrcState;
        Rhi::AccessLayoutStage NewSrcState;

        Rhi::AccessLayoutStage OldDstState{};
        Rhi::AccessLayoutStage NewDstState{};

        std::span<const Rhi::TextureSubresource> SrcSubresources;
        std::span<const Rhi::TextureSubresource> DstSubresources;

        uint32_t SrcMipLevel = 0;
        uint32_t DstMipLevel = 0;

        bool EnableAlpha    : 1 = false;
        bool SwapRBChannels : 1 = false;

        // True to place a barrier before the blit operation. if false, the caller must ensure the textures are in the correct state.
        bool SrcTransition : 1 = true;
        // True to place a barrier before the blit operation. if false, the caller must ensure the textures are in the correct state.
        bool DstTransition : 1 = true;
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
            Ref<Rhi::CommandList> CommandList;
            const BlitParameters& Parameters;

            Rhi::TextureRect SrcRect;
            Rhi::TextureRect DstRect;

            std::vector<Rhi::TextureSubresource> SrcSubresources;
            std::vector<Rhi::TextureSubresource> DstSubresources;

            OptimalBlitOperation OptimalType;

            BlitOperation(
                Rhi::CommandList&     commandList,
                const BlitParameters& parameters);
        };

    public:
        BlitRenderPass(
            Rhi::Device& rhiDevice);

    public:
        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        Co::result<void> Blit(
            const BlitParameters& parameters);

        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        void Blit(
            Rhi::CommandList&     commandList,
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
        /// Transition the textures to the correct state before copy operation.
        /// </summary>
        void BlitPushCopyBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Transition the textures to the correct state after copy operation.
        /// </summary>
        void BlitPopCopyBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Blit a texture to another texture using a copy operation.
        /// </summary>
        void BlitCopy(
            BlitOperation& operation);

    private:
        /// <summary>
        /// Transition the textures to the correct state before render operation.
        /// </summary>
        void BlitPushRenderBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Transition the textures to the correct state after render operation.
        /// </summary>
        void BlitPopRenderBarrier(
            BlitOperation& operation);

        /// <summary>
        /// Blit a texture to another texture using a render operation.
        /// </summary>
        void BlitRender(
            BlitOperation& operation);

    private:
        Ref<Rhi::Device> m_RhiDevice;
    };
} // namespace Ame::Gfx::Cache