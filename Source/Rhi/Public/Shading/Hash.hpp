#pragma once

#include <Shading/Core.hpp>
#include <DiligentCore/Common/interface/HashUtils.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::ShaderCreateInfoX>
    {
        size_t operator()(const Ame::Rhi::ShaderCreateInfoX& createInfo) const
        {
            size_t hash = 0;
            if (createInfo.FilePath())
            {
                Dg::HashCombine(hash, createInfo.FilePath());
            }
            if (createInfo.SourceCode())
            {
                Dg::HashCombine(hash, Dg::ComputeHashRaw(createInfo.SourceCode(), createInfo.SourceCodeLength()));
            }
            else if (createInfo.ByteCodeSize())
            {
                Dg::HashCombine(hash, Dg::ComputeHashRaw(createInfo.ByteCode(), createInfo.ByteCodeSize()));
            }
            Dg::HashCombine(hash, createInfo.EntryPoint());
            for (const auto& [name, value] : createInfo.Macros())
            {
                Dg::HashCombine(hash, name, value);
            }
            Dg::HashCombine(hash, createInfo.ShaderType());
            Dg::HashCombine(hash, createInfo.SourceLanguage());
            Dg::HashCombine(hash, createInfo.CombinedTextureSamplers());
            Dg::HashCombine(hash, createInfo.CombinedSamplerSuffix());
            Dg::HashCombine(hash, createInfo.SourceLanguage());
            Dg::HashCombine(hash, createInfo.ShaderCompiler());
            Dg::HashCombine(hash, createInfo.HLSLVersion());
            Dg::HashCombine(hash, createInfo.GLSLVersion());
            Dg::HashCombine(hash, createInfo.GLESSLVersion());
            Dg::HashCombine(hash, createInfo.MSLVersion());
            Dg::HashCombine(hash, createInfo.CompileFlags());
            Dg::HashCombine(hash, createInfo.LoadConstantBufferReflection());
            if (createInfo.GLSLExtensions())
            {
                Dg::HashCombine(hash, createInfo.GLSLExtensions());
            }
            if (createInfo.WebGPUEmulatedArrayIndexSuffix())
            {
                Dg::HashCombine(hash, createInfo.WebGPUEmulatedArrayIndexSuffix());
            }
            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::MaterialShaderSourceStorage>
    {
        size_t operator()(const Ame::Rhi::MaterialShaderSourceStorage& storageMap) const
        {
            size_t hash = 0;
            for (auto& [type, storage] : storageMap)
            {
                Dg::HashCombine(hash, std::to_underlying(type), storage);
            }
            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::MaterialShaderStorage>
    {
        size_t operator()(const Ame::Rhi::MaterialShaderStorage& storage) const
        {
            size_t hash = 0;
            for (const auto& [type, shader] : storage)
            {
                Dg::HashCombine(hash, std::to_underlying(type));
                Dg::HashCombine(hash, shader);
            }
            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::MaterialRenderState>
    {
        size_t operator()(const Ame::Rhi::MaterialRenderState& state) const
        {
            size_t hash = 0;

            for (const auto& [type, shader] : state.Links.Sources)
            {
                Dg::HashCombine(
                    hash,
                    std::to_underlying(type),
                    shader);
            }

            for (const auto& [type, shader] : state.Links.Shaders)
            {
                Dg::HashCombine(
                    hash,
                    std::to_underlying(type),
                    shader);
            }

            for (const auto& signature : state.Signatures)
            {
                Dg::HashCombine(hash, signature->GetUniqueID());
            }

            for (const auto& format : state.RenderTargets)
            {
                Dg::HashCombine(hash, std::to_underlying(format));
            }

            Dg::HashCombine(hash,
                            state.Blend,
                            state.SampleMask,
                            state.Rasterizer,
                            state.DepthStencil,
                            state.Sample,
                            std::to_underlying(state.ShadingRateFlags),
                            std::to_underlying(state.DSFormat),
                            state.ReadOnlyDSV);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::MaterialVertexDesc>
    {
        size_t operator()(const Ame::Rhi::MaterialVertexDesc& state) const
        {
            size_t hash = 0;
            Dg::HashCombine(hash, state.Topology, state.Flags);
            return hash;
        }
    };
} // namespace std
