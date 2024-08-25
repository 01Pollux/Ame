#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Shading/Technique.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class DepthPrePass : public Rg::Pass
    {
    public:
        static constexpr Dg::TEXTURE_FORMAT DepthFormat = Dg::TEX_FORMAT_D32_FLOAT;

        struct PassData
        {
            Dg::ITextureView*           DepthView = nullptr;
            Dg::IShaderResourceBinding* Srbs[2]{};          // _FRS_Graphics, _ERS_Graphics
            Dg::IShaderResourceBinding* LightSrb = nullptr; // _LRS_Graphics
            StdLightDrawPropCategories  LightDrawProps;
        };

    public:
        DepthPrePass(
            Ecs::World* world);

    private:
        void TryCreateResources(
            const Rg::ResourceStorage& storage);

        void OnBuild(
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