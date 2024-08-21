#include <Graphics/RenderGraph/Passes/Lighting/ComputeLightCull.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>

#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Shaders/Lighting/LightFrustumCull.hpp>

namespace Ame::Gfx
{
    ComputeLightCullPass::ComputeLightCullPass()
    {
        Name("Light Frustum Cull")
            .Flags(Rg::PassFlags::Compute)
            .Build(std::bind_front(&ComputeLightCullPass::OnBuild, this))
            .Execute(std::bind_front(&ComputeLightCullPass::OnExecute, this));
    }

    //

    void ComputeLightCullPass::TryCreateResources(
        Rhi::IRhiDevice*)
    {
        if (m_PipelineState)
        {
            return;
        }
        //
        //        Rhi::RenderDeviceWithCache<> device(rhiDevice);
        //
        //        Dg::ComputePipelineStateCreateInfo computeDesc{
        // #ifndef AME_DIST
        //            "LightFrustumCullPso"
        // #endif
        //        };
        //
        //        // Ptr shader = device.CreateShader(Rhi::LightFrustumCull_ComputeShader().GetCreateInfo());
        //        Ptr<Dg::IShader>   shader;
        //        Ptr<Dg::IDataBlob> blob;
        //        rhiDevice->GetRenderDevice()->CreateShader(Rhi::LightFrustumCull_ComputeShader().GetCreateInfo(), &shader, &blob);
        //
        //        computeDesc.pCS = shader;
        //        m_PipelineState = device.CreatePipelineState(computeDesc);
    }

    //

    void ComputeLightCullPass::OnBuild(
        Rg::Resolver& resolver)
    {
        TryCreateResources(resolver.GetDevice());
    }

    void ComputeLightCullPass::OnExecute(
        const Rg::ResourceStorage&,
        Dg::IDeviceContext*)
    {
    }
} // namespace Ame::Gfx