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
    class ForwardPlus_RenderObjectsPass : public Rg::Pass
    {
    private:
        struct PassData
        {
            Dg::ITextureView* DepthView    = nullptr;
            Dg::ITextureView* RenderTarget = nullptr;
        };

    public:
        ForwardPlus_RenderObjectsPass(
            Ecs::World* world);

    private:
        void TryCreateResources(
            const Rg::ResourceStorage&  storage,
            Dg::IShaderResourceBinding* srb);

        Co::result<void> OnBuild(
            Rg::Resolver& resolver);

        void OnExecute(
            const Rg::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ecs::World*                 m_World;
        PassData                    m_PassData;
        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx