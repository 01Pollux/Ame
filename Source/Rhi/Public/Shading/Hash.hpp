#pragma once

#include <Shading/Core.hpp>
#include <DiligentCore/Common/interface/HashUtils.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::MaterialShaderDesc>
    {
        size_t operator()(const Ame::Rhi::MaterialShaderDesc& storage) const
        {
            size_t hash = 0;
            Dg::HashCombine(hash, storage.PreShaderCode);
            Dg::HashCombine(hash, storage.PostShaderCode);
            for (auto& [key, value] : storage.Macros)
            {
                Dg::HashCombine(hash, key, value);
            }
            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::MaterialLinkShaderDesc>
    {
        size_t operator()(const Ame::Rhi::MaterialLinkShaderDesc& storage) const
        {
            size_t hash = 0;
            Dg::HashCombine(hash, storage.ShaderCode);
            for (auto& [key, value] : storage.Macros)
            {
                Dg::HashCombine(hash, key, value);
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

    template<>
    struct hash<Ame::Rhi::MaterialLinkShaderSourceStorage>
    {
        size_t operator()(const Ame::Rhi::MaterialLinkShaderSourceStorage& storageMap) const
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
    struct hash<Ame::Rhi::MaterialDesc>
    {
        size_t operator()(const Ame::Rhi::MaterialDesc& materialDesc) const
        {
            size_t hash = 0;
            Dg::HashCombine(hash,
                            materialDesc.Blend,
                            materialDesc.SampleMask,
                            materialDesc.Rasterizer,
                            materialDesc.DepthStencil,
                            materialDesc.ShaderSources);
            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::MaterialRenderState>
    {
        size_t operator()(const Ame::Rhi::MaterialRenderState& state) const
        {
            size_t hash = 0;

            for (const auto& [type, shader] : state.Links.ShaderSources)
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
                            state.Sample,
                            std::to_underlying(state.ShadingRateFlags),
                            std::to_underlying(state.DepthStencil),
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
