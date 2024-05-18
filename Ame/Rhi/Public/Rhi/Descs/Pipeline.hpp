#pragma once

#include <Rhi/Descs/Layout.hpp>

namespace Ame::Rhi
{
    using Viewport             = nri::Viewport;
    using Scissor              = nri::Rect;
    using TopologyType         = nri::Topology;
    using PrimitiveRestartType = nri::PrimitiveRestart;
    using FillMode             = nri::FillMode;
    using CullMode             = nri::CullMode;
    using BlendFactor          = nri::BlendFactor;
    using BlendFunc            = nri::BlendFunc;
    using ColorWriteBits       = nri::ColorWriteBits;
    using CompareFunc          = nri::CompareFunc;
    using StencilFunc          = nri::StencilFunc;
    using LogicFunc            = nri::LogicFunc;
    using VertexStreamStepRate = nri::VertexStreamStepRate;
    using DescriptorType       = nri::DescriptorType;

    using VertexAttributeDesc = nri::VertexAttributeDesc;
    using VertexStreamDesc    = nri::VertexStreamDesc;
    using VertexInputDesc     = nri::VertexInputDesc;

    //

    struct InputAssemblyDesc
    {
        TopologyType         Topology            = TopologyType::TRIANGLE_LIST;
        uint8_t              TessControlPointNum = 0;
        PrimitiveRestartType PrimitiveRestart    = PrimitiveRestartType::DISABLED;
    };

    struct RasterizationDesc
    {
        float DepthBias            = 0.f;
        float DepthBiasClamp       = 0.f;
        float DepthBiasSlopeFactor = 0.f;

        uint8_t ViewportNum    = 1;
        bool    DepthClamp : 1 = false;

        FillMode Fill : 1 = FillMode::SOLID;
        CullMode Cull : 2 = CullMode::BACK;

        bool FrontCounterClockwise : 1 = false;

        // Requires "isLineSmoothingSupported"
        bool AntialiasedLines : 1 = false;
        // Requires "conservativeRasterTier > 0"
        bool ConservativeRasterization : 1 = false;
    };
    static_assert(std::to_underlying(FillMode::MAX_NUM) <= (1 << 1), "Invalid bit size for FillMode");
    static_assert(std::to_underlying(CullMode::MAX_NUM) <= (1 << 2), "Invalid bit size for CullMode");

    struct MultisampleDesc
    {
        uint32_t SampleMask                = 0;
        uint8_t  SampleCount               = 1;
        bool     AlphaToCoverageEnable : 1 = false;
        // Requires "isSampleLocationSupported"
        bool ProgrammableSampleLocations : 1 = false;
    };

    struct BlendingDesc
    {
        BlendFactor Src  : 5 = BlendFactor::ONE;
        BlendFactor Dst  : 5 = BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
        BlendFunc   Func : 3 = BlendFunc::ADD;
    };
    static_assert(std::to_underlying(BlendFactor::MAX_NUM) <= (1 << 5), "Invalid bit size for BlendFactor");
    static_assert(std::to_underlying(BlendFunc::MAX_NUM) <= (1 << 3), "Invalid bit size for BlendFunc");

    struct RenderTargetDesc
    {
        ResourceFormat Format = ResourceFormat::UNKNOWN;
        BlendingDesc   Color;
        BlendingDesc   Alpha;
        ColorWriteBits WriteMask   : 4 = ColorWriteBits::RGBA;
        bool           BlendEnable : 1 = false;
    };
    static_assert(std::to_underlying(ColorWriteBits::RGBA) <= ((1 << 4) - 1), "Invalid bit size for ColorWriteBits");

    struct DepthTargetDesc
    {
        CompareFunc Func        : 5 = CompareFunc::NONE;
        bool        WriteEnable : 1 = false;
        // Requires "isDepthBoundsTestSupported", expects "CmdSetDepthBounds"
        bool BoundsTestEnable : 1 = false;
    };
    static_assert(std::to_underlying(CompareFunc::MAX_NUM) <= (1 << 5), "Invalid bit size for CompareFunc");

    struct StencilDesc
    {
        uint8_t     WriteMask       = 0xFF;
        uint8_t     CompareMask     = 0xFF;
        CompareFunc Func        : 5 = CompareFunc::NONE;
        StencilFunc OnFail      : 3 = StencilFunc::KEEP;
        StencilFunc OnDepthFail : 3 = StencilFunc::KEEP;
        StencilFunc OnPass      : 3 = StencilFunc::KEEP;
    };
    static_assert(std::to_underlying(CompareFunc::MAX_NUM) <= (1 << 5), "Invalid bit size for CompareFunc");
    static_assert(std::to_underlying(StencilFunc::MAX_NUM) <= (1 << 3), "Invalid bit size for StencilFunc");

    struct StencilTargetDesc
    {
        StencilDesc Front;
        StencilDesc Back;
    };

    struct OutputMergerDesc
    {
        std::span<const RenderTargetDesc> RenderTargets;
        ResourceFormat                    DepthStencilFormat = ResourceFormat::UNKNOWN;
        DepthTargetDesc                   DepthTarget;
        StencilTargetDesc                 StencilTarget;
        LogicFunc                         ColorLogicFunc = LogicFunc::NONE;
    };

    //

    struct GraphicsPipelineDesc
    {
        Ptr<PipelineLayout> Layout;

        InputAssemblyDesc InputAssembly;
        RasterizationDesc Rasterizer;
        OutputMergerDesc  OutputMerger;

        std::span<const ShaderDesc> Shaders;

        // Optional
        const VertexInputDesc* VertexInput = nullptr;
        // Optional
        const MultisampleDesc* Multisample = nullptr;
    };

    struct ComputePipelineDesc
    {
        Ptr<PipelineLayout> Layout;
        ShaderDesc          Shader;
    };
} // namespace Ame::Rhi