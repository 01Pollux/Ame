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
    class RenderIndirectCommandsPass : public RG::Pass
    {
    public:
        static inline const RG::ResourceViewId RGRenderInstances       = c_RGRenderInstances("Render Indirect Commands");
        static inline const RG::ResourceViewId RGSortedRenderInstances = c_RGSortedRenderInstances("Render Indirect Commands");
        static inline const RG::ResourceViewId RGDrawCommands          = c_RGDrawCommands("Render Indirect Commands");
        static inline const RG::ResourceViewId RGDrawCommandCounts     = c_RGDrawCommandCounts("Render Indirect Commands");
        static inline const RG::ResourceViewId RGRenderTarget          = c_RGFinalImage("Render Indirect Commands");

    public:
        RenderIndirectCommandsPass(
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
        Ptr<Ecs::World> m_World;

        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx