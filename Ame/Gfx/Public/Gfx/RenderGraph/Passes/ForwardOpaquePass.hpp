#pragma once

#include <RG/Pass.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/Cache/MaterialBindingCache.hpp>
#include <Gfx/Cache/CommonShader.hpp>

#include <Gfx/RenderGraph/Resources/Names.hpp>

namespace Ame::Gfx
{
    class EntityCompositor;
    namespace Cache
    {
        class CommonPipelineState;
    } // namespace Cache
} // namespace Ame::Gfx

namespace Ame::Gfx
{
    class ForwardOpaquePass : public RG::Pass
    {
    public:
        struct Input
        {
        };

        struct Output
        {
            static inline const String         c_OutputImageName{ "_OpaqueOut" };
            static inline const RG::ResourceId c_OutputImage{ c_OutputImageName };
        };

    public:
        ForwardOpaquePass(
            EntityCompositor&            entityCompositor,
            Cache::CommonShader&         commonShaders,
            Cache::MaterialBindingCache& materialCache);

    private:
        Ref<Cache::CommonShader>         m_CommonShaders;
        Ref<Cache::MaterialBindingCache> m_MaterialCache;
    };
} // namespace Ame::Gfx