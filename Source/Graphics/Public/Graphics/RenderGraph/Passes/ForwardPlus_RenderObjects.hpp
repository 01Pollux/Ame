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
        static inline const RG::ResourceViewId RGRenderInstances       = c_RGRenderInstances("Render Objects");
        static inline const RG::ResourceViewId RGSortedRenderInstances = c_RGSortedRenderInstances("Render Objects");

        static inline const RG::ResourceViewId RGRenderTarget = c_RGFinalImage("Render Objects");

    public:
        ForwardPlus_RenderObjects(
            Ecs::World* world);

    private:
        void CreateResources(
            Dg::IRenderDevice* renderDevice,
            Dg::TEXTURE_FORMAT rtvFormat);

        void Build(
            RG::Resolver& resolver);

        void Execute(
            const RG::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ecs::World* m_World;

        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx