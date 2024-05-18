#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    PipelineState::PipelineState(
        Device&             RhiDevice,
        Ptr<PipelineLayout> Layout,
        nri::Pipeline&      Pipeline) :
        m_RhiDevice(RhiDevice),
        m_Layout(std::move(Layout)),
        m_Pipeline(Pipeline)
    {
    }

    PipelineState::~PipelineState()
    {
        m_RhiDevice.Release(Unwrap());
    }

    void PipelineState::SetName(
        const char* Name) const
    {
        m_RhiDevice.SetName(Unwrap(), Name);
    }

    nri::Pipeline& PipelineState::Unwrap() const
    {
        return m_Pipeline;
    }

    Ptr<PipelineLayout> PipelineState::GetLayout() const
    {
        return m_Layout;
    }

    //

    [[nodiscard]] static nri::InputAssemblyDesc Convert(
        const InputAssemblyDesc& Desc)
    {
        return {
            .topology            = Desc.Topology,
            .tessControlPointNum = Desc.TessControlPointNum,
            .primitiveRestart    = Desc.PrimitiveRestart
        };
    }

    [[nodiscard]] static nri::RasterizationDesc Convert(
        const RasterizationDesc& Desc)
    {
        return {
            .viewportNum               = Desc.ViewportNum,
            .depthBias                 = Desc.DepthBias,
            .depthBiasClamp            = Desc.DepthBiasClamp,
            .depthBiasSlopeFactor      = Desc.DepthBiasSlopeFactor,
            .fillMode                  = Desc.Fill,
            .cullMode                  = Desc.Cull,
            .frontCounterClockwise     = Desc.FrontCounterClockwise,
            .depthClamp                = Desc.DepthClamp,
            .antialiasedLines          = Desc.AntialiasedLines,
            .conservativeRasterization = Desc.ConservativeRasterization
        };
    }

    [[nodiscard]] static nri::MultisampleDesc Convert(
        const MultisampleDesc& Desc)
    {
        return {
            .sampleMask                  = Desc.SampleMask,
            .sampleNum                   = Desc.SampleCount,
            .alphaToCoverage             = Desc.AlphaToCoverageEnable,
            .programmableSampleLocations = Desc.ProgrammableSampleLocations
        };
    }

    [[nodiscard]] static nri::ColorAttachmentDesc Convert(
        const RenderTargetDesc& Desc)
    {
        return {
            .format     = Desc.Format,
            .colorBlend = {
                .srcFactor = Desc.Color.Src,
                .dstFactor = Desc.Color.Dst,
                .func      = Desc.Color.Func },
            .alphaBlend     = { .srcFactor = Desc.Alpha.Src, .dstFactor = Desc.Alpha.Dst, .func = Desc.Alpha.Func },
            .colorWriteMask = Desc.WriteMask,
            .blendEnabled   = Desc.BlendEnable
        };
    }

    [[nodiscard]] static nri::DepthAttachmentDesc Convert(
        const DepthTargetDesc& Desc)
    {
        return {
            .compareFunc = Desc.Func,
            .write       = Desc.WriteEnable,
            .boundsTest  = Desc.BoundsTestEnable
        };
    }

    [[nodiscard]] static nri::StencilDesc Convert(
        const StencilDesc& Desc)
    {
        return {
            .compareFunc = Desc.Func,
            .fail        = Desc.OnFail,
            .pass        = Desc.OnPass,
            .depthFail   = Desc.OnDepthFail,
            .writeMask   = Desc.WriteMask,
            .compareMask = Desc.CompareMask
        };
    }

    [[nodiscard]] static nri::StencilAttachmentDesc Convert(
        const StencilTargetDesc& Desc)
    {
        return {
            .front = Convert(Desc.Front),
            .back  = Convert(Desc.Back)
        };
    }

    struct NriOutputMergerDesc
    {
        NriOutputMergerDesc(
            const OutputMergerDesc& Desc) :
            m_Desc(Desc)
        {
            m_BlendDescs.reserve(Desc.RenderTargets.size());
            for (const auto& RenderTarget : Desc.RenderTargets)
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
        const GraphicsPipelineDesc& Desc)
    {
        co_return CreatePipelineState(Desc);
    }

    Co::result<Ptr<PipelineState>> Device::CreatePipelineState(
        Co::executor_tag,
        Co::executor&,
        const ComputePipelineDesc& Desc)
    {
        co_return CreatePipelineState(Desc);
    }

    Ptr<PipelineState> Device::CreatePipelineState(
        const GraphicsPipelineDesc& Desc)
    {
        return GetImpl().m_GraphicsPipelineCache.Load(
            Desc,
            [this](size_t, const GraphicsPipelineDesc& Desc)
            {
                auto MultiSample = Desc.Multisample ? Convert(*Desc.Multisample) : nri::MultisampleDesc{};

                NriOutputMergerDesc OutputMergerDesc(Desc.OutputMerger);

                nri::GraphicsPipelineDesc NriDesc{
                    .pipelineLayout = &Desc.Layout->Unwrap(),
                    .vertexInput    = Desc.VertexInput,
                    .inputAssembly  = Convert(Desc.InputAssembly),
                    .rasterization  = Convert(Desc.Rasterizer),
                    .multisample    = Desc.Multisample ? &MultiSample : nullptr,
                    .outputMerger   = OutputMergerDesc.Get(),
                    .shaders        = Desc.Shaders.data(),
                    .shaderNum      = Count32(Desc.Shaders)
                };

                nri::Pipeline* Pipeline = nullptr;

                auto& Nri     = m_Impl->GetNRI();
                auto& NriCore = *Nri.GetCoreInterface();

                ThrowIfFailed(NriCore.CreateGraphicsPipeline(
                                  m_Impl->GetDevice(), NriDesc, Pipeline),
                              "Failed to create graphics pipeline");

                return std::make_shared<PipelineState>(*this, Desc.Layout, *Pipeline);
            });
    }

    Ptr<PipelineState> Device::CreatePipelineState(
        const ComputePipelineDesc& Desc)
    {
        return GetImpl().m_ComputePipelineCache.Load(
            Desc,
            [this](size_t, const ComputePipelineDesc& Desc)
            {
                nri::ComputePipelineDesc NriDesc{
                    .pipelineLayout = &Desc.Layout->Unwrap(),
                    .shader         = Desc.Shader
                };

                nri::Pipeline* Pipeline = nullptr;

                auto& Nri     = m_Impl->GetNRI();
                auto& NriCore = *Nri.GetCoreInterface();

                ThrowIfFailed(NriCore.CreateComputePipeline(
                                  m_Impl->GetDevice(), NriDesc, Pipeline),
                              "Failed to create compute pipeline");

                return std::make_shared<PipelineState>(*this, Desc.Layout, *Pipeline);
            });
    }

    //

    void Device::SetName(
        nri::Pipeline& Pipeline,
        const char*    Name) const
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetPipelineDebugName(Pipeline, Name);
    }

    void Device::Release(
        nri::Pipeline& Pipeline)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        m_Impl->DeferRelease(Pipeline);
    }

    void DeviceImpl::DeferRelease(
        nri::Pipeline& Pipeline)
    {
        m_FrameManager.DeferRelease(Pipeline);
    }
} // namespace Ame::Rhi