#include <ranges>
#include <boost/container_hash/hash.hpp>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Shading/Material.CommonState.hpp>
#include <Gfx/Cache/ShaderCache.hpp>
#include <Gfx/Constants.hpp>

#include <cryptopp/sha.h>
#include <Rhi/Hash/Shader.Crypto.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    MaterialCommonState::MaterialCommonState(
        Rhi::Device&             RhiDevice,
        Gfx::Cache::ShaderCache& ShaderCache,
        Ptr<Rhi::PipelineLayout> PipelineLayout,
        MaterialPipelineState    PipelineState) :
        m_RhiDevice(RhiDevice),
        m_ShaderCache(ShaderCache),
        m_PipelineLayout(PipelineLayout),
        m_PipelineStateDesc(std::move(PipelineState))
    {
    }

    Rhi::Device& MaterialCommonState::GetDevice() const
    {
        return m_RhiDevice;
    }

    Ptr<Rhi::PipelineLayout> MaterialCommonState::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    Co::result<Ptr<Rhi::PipelineState>> MaterialCommonState::GetPipelineState(
        const MaterialRenderState& RenderState) const
    {
        auto  Hash          = GetStateHash(RenderState);
        auto& PipelineState = m_PipelineStates[Hash];
        if (!PipelineState)
        {
            PipelineState = co_await CreatePipelineState(RenderState);
        }
        co_return PipelineState;
    }

    //

    auto MaterialCommonState::GetStateHash(
        const MaterialRenderState& RenderState) const -> PipelineStateHash
    {
        CryptoPP::SHA256 Hasher;

        for (auto RT : RenderState.RenderTargetFormats)
        {
            Util::UpdateCrypto(Hasher, std::to_underlying(RT));
        }
        Util::UpdateCrypto(Hasher, std::to_underlying(RenderState.DepthTargetFormat));

        for (auto& Shader : RenderState.ShaderLink.Shaders)
        {
            Util::UpdateCrypto(Hasher, Shader);
        }
        Util::UpdateCrypto(Hasher, RenderState.ShaderLink.CompileDesc);

        return Util::FinalizeDigest(Hasher);
    }

    //

    Co::result<Rhi::ShaderBytecode> MaterialCommonState::CreatePixelShader(
        const MaterialRenderState& RenderState) const
    {
        MaterialShaderStorage PixelShader;

        PixelShader.reserve(RenderState.ShaderLink.Shaders.size() + 1);
        PixelShader.emplace_back(m_PipelineStateDesc.FindShader(Rhi::LibraryShaderType).Borrow());

        for (auto& Shader : RenderState.ShaderLink.Shaders)
        {
            PixelShader.emplace_back(Shader.Borrow());
        }

        co_return co_await m_ShaderCache.get().Link(RenderState.ShaderLink.CompileDesc, std::move(PixelShader));
    }

    Co::result<std::vector<Rhi::ShaderDesc>> MaterialCommonState::GetShaderDescs(
        const Rhi::ShaderBytecode& PixelShader) const
    {
        std::vector<Rhi::ShaderDesc> ShaderDescs;
        ShaderDescs.reserve(m_PipelineStateDesc.Shaders.size());

        for (auto& Shader : m_PipelineStateDesc.Shaders)
        {
            if (Shader.GetStage() == Rhi::LibraryShaderType) [[unlikely]]
            {
                ShaderDescs.emplace_back(PixelShader.GetDesc());
            }
            else
            {
                ShaderDescs.emplace_back(Shader.GetDesc());
            }
        }

        co_return ShaderDescs;
    }

    //

    Co::result<Ptr<Rhi::PipelineState>> MaterialCommonState::CreatePipelineState(
        const MaterialRenderState& RenderState) const
    {
        auto& OutputMerger = m_PipelineStateDesc.OutputMerger;

        auto RenderTargets =
            RenderState.RenderTargetFormats |
            std::views::transform([&](auto RT)
                                  { return Rhi::RenderTargetDesc{
                                        .Format      = RT,
                                        .Color       = OutputMerger.RenderTarget.Color,
                                        .Alpha       = OutputMerger.RenderTarget.Alpha,
                                        .WriteMask   = OutputMerger.RenderTarget.WriteMask,
                                        .BlendEnable = OutputMerger.RenderTarget.BlendEnable
                                    }; }) |
            std::ranges::to<std::vector>();

        Rhi::MultisampleDesc MultiSample;
        if (m_PipelineStateDesc.MultiSample)
        {
            MultiSample = Rhi::MultisampleDesc{
                .SampleCount           = m_PipelineStateDesc.MultiSample->SampleCount,
                .AlphaToCoverageEnable = m_PipelineStateDesc.MultiSample->AlphaToCoverageEnable
            };
        }

        auto PixelShader = co_await CreatePixelShader(RenderState);
        auto ShaderDescs = co_await GetShaderDescs(PixelShader);

        MaterialVertexDesc VertexDesc;

        Rhi::GraphicsPipelineDesc Desc{
            .Layout        = m_PipelineLayout,
            .InputAssembly = m_PipelineStateDesc.InputAssembly,
            .Rasterizer    = m_PipelineStateDesc.Rasterizer,
            .OutputMerger{
                .RenderTargets  = RenderTargets,
                .DepthTarget    = OutputMerger.DepthTarget,
                .StencilTarget  = OutputMerger.StencilTarget,
                .ColorLogicFunc = OutputMerger.ColorLogicFunc },
            .Shaders     = ShaderDescs,
            .VertexInput = &VertexDesc,
            .Multisample = m_PipelineStateDesc.MultiSample ? &MultiSample : nullptr
        };

        Desc.Rasterizer.ViewportNum = Rhi::Count8(RenderState.RenderTargetFormats);
        if (RenderState.DepthTargetFormat != Rhi::ResourceFormat::UNKNOWN)
        {
            Desc.OutputMerger.DepthStencilFormat = RenderState.DepthTargetFormat;
        }
        else
        {
            Desc.OutputMerger.DepthTarget.WriteEnable = false;
        }

        co_return m_RhiDevice.get().CreatePipelineState(Desc);
    }
} // namespace Ame::Gfx::Shading