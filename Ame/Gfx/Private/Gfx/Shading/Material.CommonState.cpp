#include <ranges>
#include <boost/container_hash/hash.hpp>

#include <Rhi/Device/Device.hpp>
#include <Gfx/Shading/Material.CommonState.hpp>
#include <Gfx/Constants.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    namespace CS = Constants::Shading;

    MaterialCommonState::MaterialCommonState(
        Rhi::Device&             RhiDevice,
        Ptr<Rhi::PipelineLayout> PipelineLayout,
        MaterialPipelineState    PipelineState) :
        m_RhiDevice(RhiDevice),
        m_PipelineLayout(PipelineLayout),
        m_PipelineStateDesc(std::move(PipelineState))
    {
    }

    Rhi::Device& MaterialCommonState::GetDevice() const
    {
        return m_RhiDevice;
    }

    Ptr<Rhi::PipelineLayout> MaterialCommonState::GetPipelineLayout() const
    {
        return m_PipelineLayout;
    }

    Co::result<Ptr<Rhi::PipelineState>> MaterialCommonState::GetPipelineState(
        const MaterialRenderState& RenderState) const
    {
        auto  Hash          = GetStateHash(RenderState);
        auto& PipelineState = m_PipelineStates[Hash];
        if (!PipelineState)
        {
            PipelineState = co_await CreatePipelineState(m_RhiDevice, m_PipelineLayout, m_PipelineStateDesc, RenderState);
        }
        co_return PipelineState;
    }

    //

    auto MaterialCommonState::GetStateHash(
        const MaterialRenderState& RenderState) -> PipelineStateHash
    {
        PipelineStateHash Hash = 0;
        for (auto RT : RenderState.RenderTargetFormats)
        {
            boost::hash_combine(Hash, std::to_underlying(RT));
        }
        boost::hash_combine(Hash, std::to_underlying(RenderState.DepthTargetFormat));
        return Hash;
    }

    //

    Co::result<Ptr<Rhi::PipelineState>> MaterialCommonState::CreatePipelineState(
        Rhi::Device&                 RhiDevice,
        Ptr<Rhi::PipelineLayout>     PipelineLayout,
        const MaterialPipelineState& PipelineState,
        const MaterialRenderState&   RenderState)
    {
        auto& OutputMerger = PipelineState.OutputMerger;

        std::vector<Rhi::RenderTargetDesc> RenderTargets;
        RenderTargets.reserve(RenderState.RenderTargetFormats.size());
        for (uint32_t i = 0; i < RenderState.RenderTargetFormats.size(); i++)
        {
            RenderTargets.emplace_back(
                Rhi::RenderTargetDesc{
                    .Format      = RenderState.RenderTargetFormats[i],
                    .Color       = OutputMerger.RenderTargets[i].Color,
                    .Alpha       = OutputMerger.RenderTargets[i].Alpha,
                    .WriteMask   = OutputMerger.RenderTargets[i].WriteMask,
                    .BlendEnable = OutputMerger.RenderTargets[i].BlendEnable });
        }

        Rhi::MultisampleDesc MultiSample;
        if (PipelineState.MultiSample)
        {
            MultiSample = Rhi::MultisampleDesc{
                .SampleCount           = PipelineState.MultiSample->SampleCount,
                .AlphaToCoverageEnable = PipelineState.MultiSample->AlphaToCoverageEnable
            };
        }

        auto ShaderDescs =
            PipelineState.Shaders |
            std::views::transform([](const Rhi::ShaderBytecode& Bytecode) -> Rhi::ShaderDesc
                                  { return Bytecode.GetDesc(); }) |
            std::ranges::to<std::vector>();

        CS::MaterialVertexDesc VertexDesc;

        Rhi::GraphicsPipelineDesc Desc{
            .Layout        = PipelineLayout,
            .InputAssembly = PipelineState.InputAssembly,
            .Rasterizer    = PipelineState.Rasterizer,
            .OutputMerger{
                .RenderTargets      = RenderTargets,
                .DepthStencilFormat = RenderState.DepthTargetFormat,
                .DepthTarget        = OutputMerger.DepthTarget,
                .StencilTarget      = OutputMerger.StencilTarget,
                .ColorLogicFunc     = OutputMerger.ColorLogicFunc },
            .Shaders     = ShaderDescs,
            .VertexInput = &VertexDesc,
            .Multisample = PipelineState.MultiSample ? &MultiSample : nullptr
        };

        co_return RhiDevice.CreatePipelineState(Desc);
    }
} // namespace Ame::Gfx::Shading