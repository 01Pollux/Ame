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
    private:
        struct PassData
        {
            Dg::IBufferView* LightIds = nullptr;

#ifndef AME_DIST
#endif
        };

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
        PassData m_PassData;

        Ptr<Dg::IPipelineState>         m_PipelineState;
        Ptr<Dg::IShaderResourceBinding> m_Srb;

        Dg::IShaderResourceVariable* m_LightIndices = nullptr;
        Dg::IShaderResourceVariable* m_DepthTexture = nullptr;

        Dg::IShaderResourceVariable* m_LightIndices_Transparent = nullptr;
        Dg::IShaderResourceVariable* m_LightIndices_Opaque      = nullptr;

        Dg::IShaderResourceVariable* m_LightTexture_Transparent = nullptr;
        Dg::IShaderResourceVariable* m_LightTexture_Opaque      = nullptr;

#ifndef AME_DIST
        Dg::IShaderResourceVariable* m_DebugTexture = nullptr;
#endif
    };
} // namespace Ame::Gfxs