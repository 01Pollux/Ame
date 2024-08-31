#include <ranges>

#include <CommonStates/CommonRenderPasses/BlitRenderPass.hpp>
#include <Rhi/Utils/DeviceWithCache.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentCore/Common/interface/HashUtils.hpp>

namespace Ame::Rhi
{
    inline constexpr const char c_BlitShader[] = R"(
		Texture2D<float4> _Texture;
		SamplerState      _Sampler;

		struct VSOutput
		{
			float4 Position : SV_POSITION;
			float2 TexCoord : TEXCOORD;
		};

        VSOutput VS_Main(uint vertexID : SV_VertexID) 
		{
            VSOutput output;
	        uint u = vertexID & 1;
	        uint v = (vertexID >> 1) & 1;

	        output.Position = float4(float(u) * 2 - 1, 1 - float(v) * 2, 0, 1);
	        output.TexCoord = float2(u, v);
            return output;
		}

		float4 PS_Main(VSOutput input) : SV_TARGET
		{
			return _Texture.Sample(_Sampler, input.TexCoord);
		}
)";

    //

    BlitRenderPass::BlitRenderPass(Rhi::IRhiDevice* rhiDevice) : m_RhiDevice(rhiDevice)
    {
    }

    auto BlitRenderPass::GetPipelineState(const BlitDrawParameters& parameters) -> const PsoCacheData&
    {
        Dg::TEXTURE_FORMAT format = parameters.TargetView->GetDesc().Format;

        PsoCacheKey key(parameters, format);
        auto&       cache = m_PsoCache[key];
        if (!cache)
        {
            Rhi::RenderDeviceWithCache renderDevice(m_RhiDevice);

            Dg::ShaderCreateInfo shaderDesc;
            shaderDesc.Source       = c_BlitShader;
            shaderDesc.SourceLength = sizeof(c_BlitShader);

            shaderDesc.Desc.ShaderType = Dg::SHADER_TYPE_PIXEL;
            shaderDesc.EntryPoint      = "PS_Main";
            auto pixelShader           = GetShader(shaderDesc);

            shaderDesc.Desc.ShaderType = Dg::SHADER_TYPE_VERTEX;
            shaderDesc.EntryPoint      = "VS_Main";
            auto vertexShader          = GetShader(shaderDesc);

            Dg::ShaderResourceVariableDesc textures[]{ { Dg::SHADER_TYPE_PIXEL, "_Texture",
                                                         Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC } };
            Dg::ImmutableSamplerDesc       samplers[]{ { Dg::SHADER_TYPE_PIXEL, "_Sampler", Dg::Sam_LinearClamp } };

            Dg::GraphicsPipelineStateCreateInfo desc;

            desc.PSODesc.PipelineType = Dg::PIPELINE_TYPE_GRAPHICS;
#ifndef AME_DIST
            desc.PSODesc.Name = "BlitRenderPass";
#endif

            desc.PSODesc.ResourceLayout.Variables    = textures;
            desc.PSODesc.ResourceLayout.NumVariables = std::size(textures);

            desc.PSODesc.ResourceLayout.ImmutableSamplers    = samplers;
            desc.PSODesc.ResourceLayout.NumImmutableSamplers = std::size(samplers);

            desc.GraphicsPipeline.NumRenderTargets           = 1;
            desc.GraphicsPipeline.PrimitiveTopology          = Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            desc.GraphicsPipeline.RasterizerDesc             = Dg::RS_SolidFillNoCull;
            desc.GraphicsPipeline.DepthStencilDesc           = Dg::DSS_DisableDepth;
            desc.GraphicsPipeline.RTVFormats[0]              = format;
            desc.GraphicsPipeline.BlendDesc.RenderTargets[0] = parameters.BlendTarget;

            desc.pPS = pixelShader;
            desc.pVS = vertexShader;

            cache.Pso = renderDevice.CreateGraphicsPipelineState(desc);
            cache.Pso->CreateShaderResourceBinding(&cache.Srb, true);
        }
        return cache;
    }

    Ptr<Dg::IShader> BlitRenderPass::GetShader(const Dg::ShaderCreateInfo& desc)
    {
        Rhi::RenderDeviceWithCache renderDevice(m_RhiDevice);
        return renderDevice.CreateShader(desc);
    }

    size_t BlitRenderPass::PsoCacheKey::Hash::operator()(const PsoCacheKey& other) const
    {
        size_t hash = 0;
        Dg::HashCombine(hash, other.Format);
        Dg::HashCombine(hash, other.BlendTarget.BlendEnable);
        Dg::HashCombine(hash, other.BlendTarget.LogicOperationEnable);
        Dg::HashCombine(hash, other.BlendTarget.SrcBlend);
        Dg::HashCombine(hash, other.BlendTarget.DestBlend);
        Dg::HashCombine(hash, other.BlendTarget.BlendOp);
        Dg::HashCombine(hash, other.BlendTarget.SrcBlendAlpha);
        Dg::HashCombine(hash, other.BlendTarget.DestBlendAlpha);
        Dg::HashCombine(hash, other.BlendTarget.BlendOpAlpha);
        Dg::HashCombine(hash, other.BlendTarget.LogicOp);
        Dg::HashCombine(hash, other.BlendTarget.RenderTargetWriteMask);
        Dg::HashCombine(hash, other.SwapRBChannels);
        return hash;
    }
} // namespace Ame::Rhi