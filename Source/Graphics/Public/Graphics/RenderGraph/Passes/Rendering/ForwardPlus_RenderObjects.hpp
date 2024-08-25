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
    class ForwardPlus_RenderObjectsPass : public Rg::Pass
    {
    private:
        struct PassData
        {
            Rg::ResourceViewId DepthView;
            Rg::ResourceViewId RenderTarget;

            Rg::ResourceViewId LightIndices_Opaque;
            Rg::ResourceViewId LightHeads_Opaque;

            Rg::ResourceViewId LightIndices_Transparent;
            Rg::ResourceViewId LightHeads_Transparent;

            Dg::IShaderResourceBinding* Srbs[2]{};          // _FRS_Graphics, _ERS_Graphics
            Dg::IShaderResourceBinding* LightSrb = nullptr; // _LRS_Graphics

            void Reset()
            {
                Srbs[0] = nullptr;
            }

            operator bool() const
            {
                return Srbs[0] != nullptr;
            }
        };

    public:
        ForwardPlus_RenderObjectsPass(
            Ecs::World* world);

    private:
        void BindResourceOnce(const Rg::ResourceStorage& storage);
        void CreateResourcesOnce(const Rg::ResourceStorage& storage);

    private:
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