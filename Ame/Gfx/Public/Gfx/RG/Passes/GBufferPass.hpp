#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/Cache/MaterialBindingCache.hpp>

namespace Ame::Gfx::Cache
{
    class PipelineStateCache;
} // namespace Ame::Gfx::Cache

namespace Ame::Gfx::RG::Std
{
    class GBufferPass : public Pass
    {
    public:
        static constexpr std::array GBufferFormats{
            Rhi::ResourceFormat::RGBA8_UNORM, // Normal + Metallic
            Rhi::ResourceFormat::RGBA8_UNORM, // Base Color + Roughness
            Rhi::ResourceFormat::RGBA8_UNORM, // Emissive + Ambient Occlusion
        };

        static constexpr Rhi::ResourceFormat DepthFormat = Rhi::ResourceFormat::R32_SFLOAT;

    public:
        GBufferPass(
            Cache::MaterialBindingCache& MaterialCache);

    private:
        Ref<Cache::MaterialBindingCache> m_MaterialCache;
    };
} // namespace Ame::Gfx::RG::Std