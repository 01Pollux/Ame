#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/Cache/MaterialBindingCache.hpp>
#include <Gfx/Cache/CommonShader.hpp>

namespace Ame::Gfx::Cache
{
    class CommonPipelineState;
} // namespace Ame::Gfx::Cache

namespace Ame::Gfx::RG::Std
{
    class GBufferPass : public Pass
    {
    public:
        static constexpr std::array RenderTargetFormats{
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_Normal_Metallic
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_BaseColor_Roughness
            Rhi::ResourceFormat::RGBA8_UNORM  // GBuffer_Emissive_AmbientOcclusion
        };

        static inline const std::array RenderTargetIds{
            ResourceId("GBuffer_Normal_Metallic"),
            ResourceId("GBuffer_BaseColor_Roughness"),
            ResourceId("GBuffer_Emissive_AmbientOcclusion")
        };

        static constexpr Rhi::ResourceFormat DepthTargetFormat{
            Rhi::ResourceFormat::D32_SFLOAT // GBuffer_Depth
        };

        static inline const ResourceId DepthTargetId{
            "GBuffer_Depth"
        };

    public:
        GBufferPass(
            Cache::CommonShader&         CommonShaders,
            Cache::MaterialBindingCache& MaterialCache);

    private:
        Ref<Cache::CommonShader>         m_CommonShaders;
        Ref<Cache::MaterialBindingCache> m_MaterialCache;
    };
} // namespace Ame::Gfx::RG::Std