#pragma once

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Descs/Pipeline.hpp>
#include <Rhi/Resource/Shader.hpp>

namespace Ame::Gfx::Shading
{
    class Material;

    //

    enum class PrimitiveRestartType : uint8_t
    {
        Disabled,
        MaxUint32,
        MaxUint16
    };

    //

    using InputAssemblyState = Rhi::InputAssemblyDesc;

    struct RasterizerState : Rhi::RasterizationDesc
    {
    private:
        using RasterizationDesc::ViewportNum;
    };

    struct MultiSampleState
    {
        uint8_t SampleCount               = 1;
        bool    AlphaToCoverageEnable : 1 = false;
    };

    struct RenderTargetState
    {
        Rhi::BlendingDesc   Color;
        Rhi::BlendingDesc   Alpha;
        Rhi::ColorWriteBits WriteMask       = Rhi::ColorWriteBits::RGBA;
        bool                BlendEnable : 1 = false;
    };

    struct OutputMergerState
    {
        std::vector<RenderTargetState> RenderTargets;
        Rhi::DepthTargetDesc           DepthTarget;
        Rhi::StencilTargetDesc         StencilTarget;
        Rhi::LogicFunc                 ColorLogicFunc = Rhi::LogicFunc::NONE;
    };

    using MaterialShaderStorage = std::vector<Rhi::ShaderBytecode>;

    //

    struct MaterialPipelineState
    {
        InputAssemblyState    InputAssembly;
        RasterizerState       Rasterizer;
        OutputMergerState     OutputMerger;
        Opt<MultiSampleState> MultiSample;
        MaterialShaderStorage Shaders;
    };

    //

    struct MaterialRenderState
    {
        std::span<Rhi::ResourceFormat> RenderTargetFormats;
        Rhi::ResourceFormat            DepthTargetFormat = Rhi::ResourceFormat::UNKNOWN;
    };
} // namespace Ame::Gfx::Shading