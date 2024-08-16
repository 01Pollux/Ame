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
    class DepthPrePass : public RG::Pass
    {
    public:
        DepthPrePass(
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