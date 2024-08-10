#pragma once

#include <unordered_map>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    using BlitCopyParameters = Dg::CopyTextureAttribs;

    struct BlitDrawParameters
    {
        Dg::ITextureView*                  SourceView;
        Dg::RESOURCE_STATE_TRANSITION_MODE SrcTransition = Dg::RESOURCE_STATE_TRANSITION_MODE_NONE;

        Dg::ITextureView*                  TargetView;
        Dg::RESOURCE_STATE_TRANSITION_MODE DstTranisiton = Dg::RESOURCE_STATE_TRANSITION_MODE_NONE;

        Dg::RenderTargetBlendDesc BlendTarget;
        bool                      SwapRBChannels : 1 = false;
    };

    class BlitRenderPass
    {
    public:
        BlitRenderPass(
            Rhi::IRhiDevice* rhiDevice);

    public:
        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        void Blit(
            const BlitCopyParameters& parameters);

        /// <summary>
        /// Blit a texture to another texture.
        /// </summary>
        void Blit(
            Dg::IDeviceContext*       deviceContext,
            const BlitCopyParameters& parameters);

    public:
        /// <summary>
        /// Blit a texture to render target.
        /// </summary>
        void Blit(
            const BlitDrawParameters& parameters);

        /// <summary>
        /// Blit a texture to render target.
        /// </summary>
        void Blit(
            Dg::IDeviceContext*       deviceContext,
            const BlitDrawParameters& parameters);

    private:
        struct PsoCacheKey
        {
            Dg::TEXTURE_FORMAT        Format;
            Dg::RenderTargetBlendDesc BlendTarget;
            bool                      SwapRBChannels : 1 = false;

            PsoCacheKey(
                const BlitDrawParameters& parameters,
                Dg::TEXTURE_FORMAT        format) :
                Format(format),
                BlendTarget(parameters.BlendTarget),
                SwapRBChannels(parameters.SwapRBChannels)
            {
            }

            bool operator==(const PsoCacheKey& other) const
            {
                return Format == other.Format &&
                       SwapRBChannels == other.SwapRBChannels &&
                       BlendTarget == other.BlendTarget;
            }

            struct Hash
            {
                size_t operator()(const PsoCacheKey&) const;
            };
        };

        struct PsoCacheData
        {
            Ptr<Dg::IPipelineState>         Pso;
            Ptr<Dg::IShaderResourceBinding> Srb;

            explicit operator bool() const
            {
                return Pso;
            }
        };

        using PsoCache = std::unordered_map<PsoCacheKey, PsoCacheData, PsoCacheKey::Hash>;

        /// <summary>
        /// Get or create a pipeline state for blit draw.
        /// </summary>
        [[nodiscard]] const PsoCacheData& GetPipelineState(
            const BlitDrawParameters& parameters);

        /// <summary>
        /// Get or create a shader.
        /// </summary>
        [[nodiscard]] Ptr<Dg::IShader> GetShader(
            const Dg::ShaderCreateInfo& desc);

    private:
        Rhi::IRhiDevice* m_RhiDevice;
        PsoCache         m_PsoCache;
    };
} // namespace Ame::Rhi