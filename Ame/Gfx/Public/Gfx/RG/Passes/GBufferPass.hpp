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
        static constexpr std::array c_RenderTargetFormats{
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_Normal_Metallic
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_BaseColor_Roughness
            Rhi::ResourceFormat::RGBA8_UNORM  // GBuffer_Emissive_AmbientOcclusion
        };

        static inline const std::array c_RenderTargetIds{
            ResourceId("GBuffer_Normal_Metallic"),          // RGBA8_UNORM
            ResourceId("GBuffer_BaseColor_Roughness"),      // RGBA8_UNORM
            ResourceId("GBuffer_Emissive_AmbientOcclusion") // RGBA8_UNORM
        };

        static constexpr Rhi::ResourceFormat c_DepthTargetFormat{
            Rhi::ResourceFormat::D32_SFLOAT // GBuffer_Depth
        };

        static inline const ResourceId c_DepthTargetId{
            "GBuffer_Depth" // D32_SFLOAT
        };

    public:
        GBufferPass(
            Cache::CommonShader&         commonShaders,
            Cache::MaterialBindingCache& materialCache);

    private:
        Ref<Cache::CommonShader>         m_CommonShaders;
        Ref<Cache::MaterialBindingCache> m_MaterialCache;
    };
} // namespace Ame::Gfx::RG::Std