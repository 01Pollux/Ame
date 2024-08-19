#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Shading/Technique.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class DepthPrePass : public Rg::Pass
    {
    public:
        DepthPrePass(
            Ecs::World* world);

    private:
        void TryCreateResources(
            const Rg::ResourceStorage&  storage,
            Dg::IShaderResourceBinding* srb);

        void OnBuild(
            Rg::Resolver& resolver);

        void OnExecute(
            const Rg::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ecs::World* m_World;

        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx