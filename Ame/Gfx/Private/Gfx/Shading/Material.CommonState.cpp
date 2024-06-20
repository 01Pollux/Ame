#include <ranges>
#include <boost/container_hash/hash.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Hash/Pipeline.hpp>

#include <Gfx/Shading/Material.CommonState.hpp>
#include <Gfx/Cache/ShaderCache.hpp>
#include <Gfx/Cache/CommonPipelineState.hpp>
#include <Gfx/Constants.hpp>

#include <cryptopp/sha.h>
#include <Rhi/Hash/Shader.Crypto.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    MaterialCommonState::MaterialCommonState(
        Rhi::DeviceResourceAllocator&  allocator,
        Cache::ShaderCache&            shaderCache,
        Cache::CommonPipelineState&    pipelineStateCache,
        Ptr<Rhi::ScopedPipelineLayout> pipelineLayout,
        MaterialPipelineState          pipelineState) :
        m_Allocator(allocator),
        m_ShaderCache(shaderCache),
        m_PipelineStateCahe(pipelineStateCache),
        m_PipelineLayout(std::move(pipelineLayout)),
        m_PipelineStateDesc(std::move(pipelineState)),
        m_BasePipelineHash(CreatePipelineHash())
    {
    }

    Rhi::DeviceResourceAllocator& MaterialCommonState::GetAllocator() const noexcept
    {
        return m_Allocator;
    }

    Ptr<Rhi::ScopedPipelineLayout> MaterialCommonState::GetPipelineLayout() const noexcept
    {
        return m_PipelineLayout;
    }

    const MaterialPipelineState& MaterialCommonState::GetPipelineStateDesc() const noexcept
    {
        return m_PipelineStateDesc;
    }

    Co::result<Ptr<Rhi::ScopedPipelineState>> MaterialCommonState::GetPipelineState(
        const MaterialRenderState& renderState) const
    {
        auto  hash          = GetStateHash(renderState);
        auto& pipelineState = m_PipelineStates[hash];
        if (!pipelineState)
        {
            pipelineState = co_await CreatePipelineState(renderState);
        }
        co_return pipelineState;
    }

    auto MaterialCommonState::GetPipelineHash() const noexcept -> const PipelineStateHash&
    {
        return m_BasePipelineHash;
    }

    //

    auto MaterialCommonState::CreatePipelineHash() const -> PipelineStateHash
    {
        uint64_t hash = 0;

        HashCombine(hash, std::to_underlying(m_PipelineStateDesc.InputAssembly.Topology));
        HashCombine(hash, m_PipelineStateDesc.InputAssembly.TessControlPointNum);
        HashCombine(hash, std::to_underlying(m_PipelineStateDesc.InputAssembly.PrimitiveRestart));

        HashCombine(hash, static_cast<const Rhi::RasterizationDesc&>(m_PipelineStateDesc.Rasterizer));

        HashCombine(hash, m_PipelineStateDesc.OutputMerger.RenderTarget.Color);
        HashCombine(hash, m_PipelineStateDesc.OutputMerger.RenderTarget.Alpha);
        HashCombine(hash, std::to_underlying(m_PipelineStateDesc.OutputMerger.RenderTarget.WriteMask));
        HashCombine(hash, std::to_underlying(m_PipelineStateDesc.OutputMerger.RenderTarget.Mode));

        HashCombine(hash, m_PipelineStateDesc.OutputMerger.DepthTarget);
        HashCombine(hash, m_PipelineStateDesc.OutputMerger.StencilTarget);
        HashCombine(hash, std::to_underlying(m_PipelineStateDesc.OutputMerger.ColorLogicFunc));

        if (m_PipelineStateDesc.MultiSample)
        {
            HashCombine(hash, m_PipelineStateDesc.MultiSample->SampleCount);
            HashCombine(hash, m_PipelineStateDesc.MultiSample->AlphaToCoverageEnable);
        }

        CryptoPP::SHA256 hasher;
        Util::UpdateCrypto(hasher, hash);
        for (auto& shader : m_PipelineStateDesc.Shaders)
        {
            Util::UpdateCrypto(hasher, shader);
        }

        return Util::FinalizeDigest(hasher);
    }

    auto MaterialCommonState::GetStateHash(
        const MaterialRenderState& RenderState) const -> PipelineStateHash
    {
        CryptoPP::SHA256 hasher;

        for (auto format : RenderState.RenderTargetFormats)
        {
            Util::UpdateCrypto(hasher, std::to_underlying(format));
        }
        Util::UpdateCrypto(hasher, std::to_underlying(RenderState.DepthTargetFormat));

        for (auto& shader : RenderState.ShaderLink.Shaders)
        {
            Util::UpdateCrypto(hasher, shader);
        }
        Util::UpdateCrypto(hasher, RenderState.ShaderLink.CompileDesc);

        return Util::FinalizeDigest(hasher);
    }

    //

    Co::result<Rhi::ShaderBytecode> MaterialCommonState::CreatePixelShader(
        const MaterialRenderState& renderState) const
    {
        MaterialShaderStorage pixelShader;

        pixelShader.reserve(renderState.ShaderLink.Shaders.size() + 1);
        pixelShader.emplace_back(m_PipelineStateDesc.FindShader(Rhi::LibraryShaderType).Borrow());

        for (auto& Shader : renderState.ShaderLink.Shaders)
        {
            pixelShader.emplace_back(Shader.Borrow());
        }

        Rhi::ShaderLinkDesc linkDesc{ renderState.ShaderLink.CompileDesc, pixelShader };
        co_return co_await m_ShaderCache.get().Link(linkDesc);
    }

    Co::result<std::vector<Rhi::ShaderDesc>> MaterialCommonState::GetShaderDescs(
        const Rhi::ShaderBytecode& pixelShader) const
    {
        std::vector<Rhi::ShaderDesc> shaderDescs;
        shaderDescs.reserve(m_PipelineStateDesc.Shaders.size());

        for (auto& shader : m_PipelineStateDesc.Shaders)
        {
            if (shader.GetStage() == Rhi::LibraryShaderType) [[unlikely]]
            {
                shaderDescs.emplace_back(pixelShader.GetDesc());
            }
            else
            {
                shaderDescs.emplace_back(shader.GetDesc());
            }
        }

        co_return shaderDescs;
    }

    //

    Co::result<Ptr<Rhi::ScopedPipelineState>> MaterialCommonState::CreatePipelineState(
        const MaterialRenderState& renderState) const
    {
        auto& outputMerger = m_PipelineStateDesc.OutputMerger;

        auto renderTargets =
            renderState.RenderTargetFormats |
            std::views::transform([&](auto RT)
                                  { return Rhi::RenderTargetDesc{
                                        .Format      = RT,
                                        .Color       = outputMerger.RenderTarget.Color,
                                        .Alpha       = outputMerger.RenderTarget.Alpha,
                                        .WriteMask   = outputMerger.RenderTarget.WriteMask,
                                        .BlendEnable = outputMerger.RenderTarget.Mode != BlendMode::Opaque
                                    }; }) |
            std::ranges::to<std::vector>();

        Rhi::MultisampleDesc multiSample;
        if (m_PipelineStateDesc.MultiSample)
        {
            multiSample = Rhi::MultisampleDesc{
                .SampleCount           = m_PipelineStateDesc.MultiSample->SampleCount,
                .AlphaToCoverageEnable = m_PipelineStateDesc.MultiSample->AlphaToCoverageEnable
            };
        }

        auto pixelShader = co_await CreatePixelShader(renderState);
        auto shaderDescs = co_await GetShaderDescs(pixelShader);

        MaterialVertexDesc vertexDesc;

        Rhi::GraphicsPipelineDesc graphicsDesc{
            .Layout        = m_PipelineLayout->Unwrap(),
            .InputAssembly = m_PipelineStateDesc.InputAssembly,
            .Rasterizer    = m_PipelineStateDesc.Rasterizer,
            .OutputMerger{
                .RenderTargets  = renderTargets,
                .DepthTarget    = outputMerger.DepthTarget,
                .StencilTarget  = outputMerger.StencilTarget,
                .ColorLogicFunc = outputMerger.ColorLogicFunc },
            .Shaders     = shaderDescs,
            .VertexInput = &vertexDesc,
            .Multisample = m_PipelineStateDesc.MultiSample ? &multiSample : nullptr
        };

        graphicsDesc.Rasterizer.ViewportNum = Rhi::Count8(renderState.RenderTargetFormats);
        if (renderState.DepthTargetFormat != Rhi::ResourceFormat::UNKNOWN)
        {
            graphicsDesc.OutputMerger.DepthStencilFormat = renderState.DepthTargetFormat;
        }
        else
        {
            graphicsDesc.OutputMerger.DepthTarget.WriteEnable = false;
        }

        co_return co_await m_PipelineStateCahe.get().Load(graphicsDesc);
    }
} // namespace Ame::Gfx::Shading