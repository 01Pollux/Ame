#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device.Impl.hpp>

#include <Rhi/Resource/PipelineState.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
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

    Co::result<PipelineState> DeviceResourceAllocator::CreatePipelineState(
        const GraphicsPipelineDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        auto                 multiSample       = desc.Multisample ? Convert(*desc.Multisample) : nri::MultisampleDesc{};
        nri::PipelineLayout* nriPipelineLayout = desc.Layout.get().Unwrap();

        NriOutputMergerDesc OutputMergerDesc(desc.OutputMerger);

        nri::GraphicsPipelineDesc graphicsDesc{
            .pipelineLayout = nriPipelineLayout,
            .vertexInput    = desc.VertexInput,
            .inputAssembly  = Convert(desc.InputAssembly),
            .rasterization  = Convert(desc.Rasterizer),
            .multisample    = desc.Multisample ? &multiSample : nullptr,
            .outputMerger   = OutputMergerDesc.Get(),
            .shaders        = desc.Shaders.data(),
            .shaderNum      = Count32(desc.Shaders)
        };

        nri::Pipeline* nriPipeline = nullptr;
        ThrowIfFailed(nriCore.CreateGraphicsPipeline(
                          nriDevice, graphicsDesc, nriPipeline),
                      "Failed to create graphics pipeline");

        co_return PipelineState(nriCore, nriPipelineLayout, nriPipeline);
    }

    Co::result<PipelineState> DeviceResourceAllocator::CreatePipelineState(
        const ComputePipelineDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        nri::PipelineLayout* nriPipelineLayout = desc.Layout.get().Unwrap();

        nri::ComputePipelineDesc computeDesc{
            .pipelineLayout = nriPipelineLayout,
            .shader         = desc.Shader
        };

        nri::Pipeline* nriPipeline = nullptr;
        ThrowIfFailed(nriCore.CreateComputePipeline(
                          nriDevice, computeDesc, nriPipeline),
                      "Failed to create compute pipeline");

        co_return PipelineState(nriCore, nriPipelineLayout, nriPipeline);
    }

    //

    Co::result<void> DeviceResourceAllocator::Release(
        PipelineLayout& layout)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriCore          = *nri.GetCoreInterface();

        nriCore.DestroyPipelineLayout(*layout.Unwrap());
        co_return;
    }
} // namespace Ame::Rhi