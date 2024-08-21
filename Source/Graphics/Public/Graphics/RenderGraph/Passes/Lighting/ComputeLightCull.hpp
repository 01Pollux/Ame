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
    class ComputeLightCullPass : public Rg::Pass
    {
    public:
        ComputeLightCullPass();

    private:
        void TryCreateResources(
            Rhi::IRhiDevice* rhiDevice);

        void OnBuild(
            Rg::Resolver& resolver);

        void OnExecute(
            const Rg::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ptr<Dg::IPipelineState> m_PipelineState;
    };
} // namespace Ame::Gfx