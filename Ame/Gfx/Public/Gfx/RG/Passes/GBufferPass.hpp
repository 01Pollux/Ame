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
        static constexpr const char* c_Normal_Metallic           = "GBuffer_Normal_Metallic";
        static constexpr const char* c_BaseColor_Roughness       = "GBuffer_BaseColor_Roughness";
        static constexpr const char* c_Emissive_AmbientOcclusion = "GBuffer_Emissive_AmbientOcclusion";
        static constexpr const char* c_Depth                     = "GBuffer_Depth";

        static constexpr std::array c_RenderTargetFormats{
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_Normal_Metallic
            Rhi::ResourceFormat::RGBA8_UNORM, // GBuffer_BaseColor_Roughness
            Rhi::ResourceFormat::RGBA8_UNORM  // GBuffer_Emissive_AmbientOcclusion
        };

        static inline const std::array c_RenderTargetIds{
            ResourceId(c_Normal_Metallic),          // RGBA8_UNORM
            ResourceId(c_BaseColor_Roughness),      // RGBA8_UNORM
            ResourceId(c_Emissive_AmbientOcclusion) // RGBA8_UNORM
        };

        static constexpr Rhi::ResourceFormat c_DepthTargetFormat{
            Rhi::ResourceFormat::D32_SFLOAT // GBuffer_Depth
        };

        static inline const ResourceId c_DepthTargetId{
            c_Depth // D32_SFLOAT
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