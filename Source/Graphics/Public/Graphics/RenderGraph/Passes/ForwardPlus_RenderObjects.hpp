#pragma once

#include <RG/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Shading/Technique.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class ForwardPlus_RenderObjects : public RG::Pass
    {
    public:
        static inline const RG::ResourceViewId RGRenderTarget = c_RGFinalImage("Render Objects");

    public:
        ForwardPlus_RenderObjects(
            Ecs::World* world);

    private:
        void TryCreateResources(
            const RG::ResourceStorage&  storage,
            Dg::IShaderResourceBinding* srb);

        void OnBuild(
            RG::Resolver& resolver);

        void OnExecute(
            const RG::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ecs::World* m_World;

        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx