#pragma once

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Descs/Pipeline.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

#include <Gfx/Shading/Constants.hpp>

namespace Ame::Gfx::Shading
{
    class Material;

    //

    enum class PrimitiveRestartType : uint8_t
    {
        Disabled,
        MaxUint16,
        MaxUint32
    };

    enum class BlendMode : uint8_t
    {
        Opaque,
        Transparent,
        Overlay,
        Count
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
        Rhi::ColorWriteBits WriteMask = Rhi::ColorWriteBits::RGBA;
        BlendMode           Mode      = BlendMode::Opaque;
    };

    struct OutputMergerState
    {
        RenderTargetState      RenderTarget;
        Rhi::DepthTargetDesc   DepthTarget;
        Rhi::StencilTargetDesc StencilTarget;
        Rhi::LogicFunc         ColorLogicFunc = Rhi::LogicFunc::NONE;
    };

    using MaterialShaderStorage = std::vector<Rhi::ShaderBytecode>;

    //

    struct MaterialPipelineState
    {
        InputAssemblyState    InputAssembly;
        RasterizerState       Rasterizer;
        OutputMergerState     OutputMerger;
        Opt<MultiSampleState> MultiSample;

        /// <summary>
        /// The vertex shader is required.
        /// The vertex shader input layout must be same as 'MaterialVertex'.
        /// The vertex shader output layout must be same as 'VSToPS'.
        ///
        /// The pixel shader is required.
        /// The pixel shader must be a library shader.
        /// The pixel shader must have a 'PSM_Main' entry point with no [shader("...")] attribute.
        /// The pixel shader input layout must be same as 'VSToPS'.
        /// The pixel shader output layout must be same as 'MaterialFragment'.
        /// </summary>
        MaterialShaderStorage Shaders;

        [[nodiscard]] const Rhi::ShaderBytecode& FindShader(
            Rhi::ShaderType type) const;
    };

    //

    struct MaterialShaderLink
    {
        MaterialShaderStorage  Shaders;
        Rhi::ShaderCompileDesc CompileDesc;
    };

    struct MaterialRenderState
    {
        std::span<const Rhi::ResourceFormat> RenderTargetFormats;
        Rhi::ResourceFormat                  DepthTargetFormat = Rhi::ResourceFormat::UNKNOWN;
        MaterialShaderLink                   ShaderLink;
    };
} // namespace Ame::Gfx::Shading