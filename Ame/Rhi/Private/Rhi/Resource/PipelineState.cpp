#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    PipelineState::PipelineState(
        Device&             rhiDevice,
        Ptr<PipelineLayout> layout,
        nri::Pipeline&      nriPipeline) :
        m_RhiDevice(rhiDevice),
        m_Layout(std::move(layout)),
        m_Pipeline(nriPipeline)
    {
    }

    PipelineState::~PipelineState()
    {
        m_RhiDevice.Release(m_Pipeline);
    }

    void PipelineState::SetName(
        const char* name) const
    {
        m_RhiDevice.SetName(m_Pipeline, name);
    }

    const nri::Pipeline& PipelineState::Unwrap() const
    {
        return m_Pipeline;
    }

    Ptr<PipelineLayout> PipelineState::GetLayout() const
    {
        return m_Layout;
    }

    //

    [[nodiscard]] static nri::InputAssemblyDesc Convert(
        const InputAssemblyDesc& desc)
    {
        return {
            .topology            = desc.Topology,
            .tessControlPointNum = desc.TessControlPointNum,
            .primitiveRestart    = desc.PrimitiveRestart
        };
    }

    [[nodiscard]] static nri::RasterizationDesc Convert(
        const RasterizationDesc& desc)
    {
        return {
            .viewportNum               = desc.ViewportNum,
            .depthBias                 = desc.DepthBias,
            .depthBiasClamp            = desc.DepthBiasClamp,
            .depthBiasSlopeFactor      = desc.DepthBiasSlopeFactor,
            .fillMode                  = desc.Fill,
            .cullMode                  = desc.Cull,
            .frontCounterClockwise     = desc.FrontCounterClockwise,
            .depthClamp                = desc.DepthClamp,
            .antialiasedLines          = desc.AntialiasedLines,
            .conservativeRasterization = desc.ConservativeRasterization
        };
    }

    [[nodiscard]] static nri::MultisampleDesc Convert(
        const MultisampleDesc& desc)
    {
        return {
            .sampleMask                  = desc.SampleMask,
            .sampleNum                   = desc.SampleCount,
            .alphaToCoverage             = desc.AlphaToCoverageEnable,
            .programmableSampleLocations = desc.ProgrammableSampleLocations
        };
    }

    [[nodiscard]] static nri::ColorAttachmentDesc Convert(
        const RenderTargetDesc& desc)
    {
        return {
            .format     = desc.Format,
            .colorBlend = {
                .srcFactor = desc.Color.Src,
                .dstFactor = desc.Color.Dst,
                .func      = desc.Color.Func },
            .alphaBlend     = { .srcFactor = desc.Alpha.Src, .dstFactor = desc.Alpha.Dst, .func = desc.Alpha.Func },
            .colorWriteMask = desc.WriteMask,
            .blendEnabled   = desc.BlendEnable
        };
    }

    [[nodiscard]] static nri::DepthAttachmentDesc Convert(
        const DepthTargetDesc& desc)
    {
        return {
            .compareFunc = desc.Func,
            .write       = desc.WriteEnable,
            .boundsTest  = desc.BoundsTestEnable
        };
    }

    [[nodiscard]] static nri::StencilDesc Convert(
        const StencilDesc& desc)
    {
        return {
            .compareFunc = desc.Func,
            .fail        = desc.OnFail,
            .pass        = desc.OnPass,
            .depthFail   = desc.OnDepthFail,
            .writeMask   = desc.WriteMask,
            .compareMask = desc.CompareMask
        };
    }

    [[nodiscard]] static nri::StencilAttachmentDesc Convert(
        const StencilTargetDesc& desc)
    {
        return {
            .front = Convert(desc.Front),
            .back  = Convert(desc.Back)
        };
    }

    struct NriOutputMergerDesc
    {
        explicit NriOutputMergerDesc(
            const OutputMergerDesc& desc) :
            m_Desc(desc)
        {
            m_BlendDescs.reserve(desc.RenderTargets.size());
            for (const auto& RenderTarget : desc.RenderTargets)
            {
                m_BlendDescs.emplace_back(Convert(RenderTarget));
            }
        }

        [[nodiscard]] nri::OutputMergerDesc Get() const
        {
            return {
                .color              = m_BlendDescs.data(),
                .depth              = Convert(m_Desc.DepthTarget),
                .stencil            = Convert(m_Desc.StencilTarget),
                .depthStencilFormat = m_Desc.DepthStencilFormat,
                .colorLogicFunc     = m_Desc.ColorLogicFunc,
                .colorNum           = Count32(m_BlendDescs)
            };
        }

    private:
        const OutputMergerDesc&               m_Desc;
        std::vector<nri::ColorAttachmentDesc> m_BlendDescs;
    };

    //

    Co::result<Ptr<PipelineState>> Device::CreatePipelineState(
        Co::executor_tag,
        Co::executor&,
        const GraphicsPipelineDesc& desc)
    {
        co_return CreatePipelineState(desc);
    }

    Co::result<Ptr<PipelineState>> Device::CreatePipelineState(
        Co::executor_tag,
        Co::executor&,
        const ComputePipelineDesc& desc)
    {
        co_return CreatePipelineState(desc);
    }

    Ptr<PipelineState> Device::CreatePipelineState(
        const GraphicsPipelineDesc& Desc)
    {
        return GetImpl().m_GraphicsPipelineCache.Load(
            Desc,
            [this](size_t, const GraphicsPipelineDesc& Desc)
            {
                auto multiSample = Desc.Multisample ? Convert(*Desc.Multisample) : nri::MultisampleDesc{};

                NriOutputMergerDesc OutputMergerDesc(Desc.OutputMerger);

                nri::GraphicsPipelineDesc pipelineDesc{
                    .pipelineLayout = &Desc.Layout->Unwrap(),
                    .vertexInput    = Desc.VertexInput,
                    .inputAssembly  = Convert(Desc.InputAssembly),
                    .rasterization  = Convert(Desc.Rasterizer),
                    .multisample    = Desc.Multisample ? &multiSample : nullptr,
                    .outputMerger   = OutputMergerDesc.Get(),
                    .shaders        = Desc.Shaders.data(),
                    .shaderNum      = Count32(Desc.Shaders)
                };

                auto& nriUtils = m_Impl->GetNRI();
                auto& nriCore  = *nriUtils.GetCoreInterface();

                nri::Pipeline* nriPipeline = nullptr;
                ThrowIfFailed(nriCore.CreateGraphicsPipeline(
                                  m_Impl->GetDevice(), pipelineDesc, nriPipeline),
                              "Failed to create graphics pipeline");

                return std::make_shared<PipelineState>(*this, Desc.Layout, *nriPipeline);
            });
    }

    Ptr<PipelineState> Device::CreatePipelineState(
        const ComputePipelineDesc& desc)
    {
        return GetImpl().m_ComputePipelineCache.Load(
            desc,
            [this](size_t, const ComputePipelineDesc& desc)
            {
                nri::ComputePipelineDesc computeDesc{
                    .pipelineLayout = &desc.Layout->Unwrap(),
                    .shader         = desc.Shader
                };

                auto& nriUtils = m_Impl->GetNRI();
                auto& nriCore  = *nriUtils.GetCoreInterface();

                nri::Pipeline* nriPipeline = nullptr;
                ThrowIfFailed(nriCore.CreateComputePipeline(
                                  m_Impl->GetDevice(), computeDesc, nriPipeline),
                              "Failed to create compute pipeline");

                return std::make_shared<PipelineState>(*this, desc.Layout, *nriPipeline);
            });
    }

    //

    void Device::SetName(
        nri::Pipeline& nriPipeline,
        const char*    name) const
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.SetPipelineDebugName(nriPipeline, name);
    }

    void Device::Release(
        nri::Pipeline& nriPipeline)
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        m_Impl->DeferRelease(nriPipeline);
    }

    void DeviceImpl::DeferRelease(
        nri::Pipeline& nriPipeline)
    {
        m_FrameManager.DeferRelease(nriPipeline);
    }
} // namespace Ame::Rhi