#pragma once

#include <Core/Enum.hpp>
#include <Math/Vector.hpp>
#include <IO/MemStream.hpp>
#include <span>

#include <NRI/NRI.h>
#include <NRI/NRIDescs.h>

namespace Ame::Rhi
{
    class Device;
    class PipelineLayout;

    using BufferUsageBits      = nri::BufferUsageBits;
    using TextureType          = nri::TextureType;
    using TextureUsageBits     = nri::TextureUsageBits;
    using ResourceFormat       = nri::Format;
    using IndexType            = nri::IndexType;
    using Viewport             = nri::Viewport;
    using Scissor              = nri::Rect;
    using StageBits            = nri::StageBits;
    using ShaderType           = nri::StageBits;
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

    //

    struct DescriptorRangeDesc
    {
        uint32_t       Register;
        uint32_t       Count;
        DescriptorType Type;
        StageBits      Stages;
        bool           DynamicSize = false;
        bool           Array       = false;
    };

    struct DynamicConstantBufferDesc
    {
        uint32_t  Register;
        StageBits Stages;
    };

    struct DescriptorSetDesc
    {
        uint32_t                             RegisterSpace;
        std::span<DescriptorRangeDesc>       Ranges;
        std::span<DynamicConstantBufferDesc> DynamicConstantBuffers;
        bool                                 PartiallyBound = false;
    };

    struct PushConstantDesc
    {
        uint32_t  Register;
        uint32_t  Size;
        StageBits Stages;
    };

    struct PipelineLayoutDesc
    {
        std::span<DescriptorSetDesc> DescriptorSets;
        std::span<PushConstantDesc>  PushConstants;
    };

    //

    struct VertexAttributeDesc
    {
        const char*    HlslSemantic = nullptr;
        ResourceFormat Format;
        uint8_t        HlslSemanticIndex = 0;
        uint8_t        VkLocation;
        uint32_t       Offset;
        uint16_t       StreamIndex = 0;
    };

    struct VertexStreamDesc
    {
        uint32_t             Stride;
        uint32_t             BindingSlot;
        VertexStreamStepRate StepRate = VertexStreamStepRate::PER_VERTEX;
    };

    struct VertexInputDesc
    {
        std::span<VertexAttributeDesc> Attributes;
        std::span<VertexStreamDesc>    Streams;
    };

    struct InputAssemblyDesc
    {
        TopologyType         Topoly;
        uint8_t              TessControlPointNum = 0;
        PrimitiveRestartType PrimitiveRestart    = PrimitiveRestartType::DISABLED;
    };

    struct RasterizationDesc
    {
        uint32_t ViewportNum;
        float    DepthBias                 = 0.f;
        float    DepthBiasClamp            = 0.f;
        float    DepthBiasSlopeFactor      = 0.f;
        FillMode Fill                      = FillMode::SOLID;
        CullMode Cull                      = CullMode::BACK;
        bool     FrontCounterClockwise     = false;
        bool     DepthClamp                = false;
        bool     AntialiasedLines          = false; // Requires "isLineSmoothingSupported"
        bool     ConservativeRasterization = false; // Requires "conservativeRasterTier > 0"
    };

    struct MultisampleDesc
    {
        uint32_t SampleMask                  = 0;
        uint8_t  SampleNum                   = 1;
        bool     AlphaToCoverageEnable       = false;
        bool     ProgrammableSampleLocations = false; // Requires "isSampleLocationSupported"
    };

    struct BlendingDesc
    {
        BlendFactor Src  = BlendFactor::ONE;
        BlendFactor Dst  = BlendFactor::ZERO;
        BlendFunc   Func = BlendFunc::ADD;
    };

    struct RenderTargetDesc
    {
        ResourceFormat Format;
        BlendingDesc   Color;
        BlendingDesc   Alpha;
        ColorWriteBits WriteMask;
        bool           BlendEnable = false;
    };

    struct DepthTargetDesc
    {
        CompareFunc Func                 = CompareFunc::LESS;
        bool        WriteEnable      : 1 = false;
        bool        BoundsTestEnable : 1 = false; // Requires "isDepthBoundsTestSupported", expects "CmdSetDepthBounds"
    };

    struct StencilDesc
    {
        CompareFunc Func        = CompareFunc::ALWAYS;
        StencilFunc OnFail      = StencilFunc::KEEP;
        StencilFunc OnDepthFail = StencilFunc::KEEP;
        StencilFunc OnPass      = StencilFunc::KEEP;
        uint8_t     WriteMask   = 0xFF;
        uint8_t     CompareMask = 0xFF;
    };

    struct StencilTargetDesc
    {
        StencilDesc Front;
        StencilDesc Back;
    };

    struct OutputMergerDesc
    {
        std::span<RenderTargetDesc> RenderTargets;
        ResourceFormat              DepthStencilFormat = ResourceFormat::UNKNOWN;
        DepthTargetDesc             DepthTarget;
        StencilTargetDesc           StencilTarget;
        LogicFunc                   ColorLogicFunc = LogicFunc::NONE;
    };

    struct ShaderDesc
    {
        IO::MemDeviceView Bytecode;
        ShaderType        Type;
    };

    struct GraphicsPipelineDesc
    {
        const PipelineLayout* Layout = nullptr;

        InputAssemblyDesc InputAssembly;
        RasterizationDesc Rasterizer;
        OutputMergerDesc  OutputMerger;

        std::span<ShaderDesc> Shaders;

        // Optional
        VertexInputDesc* VertexInput = nullptr;
        // Optional
        const MultisampleDesc* Multisample = nullptr;
    };

    //

    enum class BufferViewType : uint8_t
    {
        ConstantBuffer,
        ShaderResource,
        UnorderedAccess,
    };

    enum class TextureViewType : uint32_t
    {
        None,

        ShaderResource1D       = 1 << 0,
        ShaderResource1DArray  = 1 << 1,
        UnorderedAccess1D      = 1 << 2,
        UnorderedAccess1DArray = 1 << 3,
        RenderTarget1D         = 1 << 4,
        DepthStencil1D         = 1 << 5,

        AnyOneDimensional =
            ShaderResource1D |
            ShaderResource1DArray |
            UnorderedAccess1D |
            UnorderedAccess1DArray |
            RenderTarget1D |
            DepthStencil1D,

        ShaderResource2D          = 1 << 6,
        ShaderResource2DArray     = 1 << 7,
        ShaderResource2DCube      = 1 << 8,
        ShaderResource2DCubeArray = 1 << 9,
        UnorderedAccess2D         = 1 << 10,
        UnorderedAccess2DArray    = 1 << 11,
        RenderTarget2D            = 1 << 12,
        DepthStencil2D            = 1 << 13,

        AnyTwoDimensional =
            ShaderResource2D |
            ShaderResource2DArray |
            ShaderResource2DCube |
            ShaderResource2DCubeArray |
            UnorderedAccess2D |
            UnorderedAccess2DArray |
            RenderTarget2D |
            DepthStencil2D,

        ShaderResource3D  = 1 << 14,
        UnorderedAccess3D = 1 << 15,
        RenderTarget3D    = 1 << 16,

        AnyThreeDimensional =
            ShaderResource3D |
            UnorderedAccess3D |
            RenderTarget3D,

        AnyShaderResource = ShaderResource1D |
                            ShaderResource1DArray |
                            ShaderResource2D |
                            ShaderResource2DArray |
                            ShaderResource2DCube |
                            ShaderResource2DCubeArray |
                            ShaderResource3D,

        AnyUnorderedAccess = UnorderedAccess1D |
                             UnorderedAccess1DArray |
                             UnorderedAccess2D |
                             UnorderedAccess2DArray |
                             UnorderedAccess3D,

        AnyRenderTarget = RenderTarget1D |
                          RenderTarget2D |
                          RenderTarget3D,

        AnyDepthStencil = DepthStencil1D |
                          DepthStencil2D
    };

    enum class TextureViewFlags : uint8_t
    {
        None            = 0,
        ReadOnlyDepth   = 1 << 0,
        ReadOnlyStencil = 1 << 1,

        ReadOnlyDepthStencil = ReadOnlyDepth | ReadOnlyStencil
    };

    //

    using Mip_t    = uint8_t;
    using Dim_t    = uint8_t;
    using Sample_t = uint8_t;

    //

    struct BufferRange
    {
        uint64_t Offset = 0;
        uint64_t Size   = 0;

        constexpr BufferRange(
            uint64_t Offset = 0,
            uint64_t Size   = 0) :
            Offset(Offset),
            Size(Size)
        {
        }

        auto operator<=>(
            const BufferRange& Other) const noexcept = default;
    };

    static constexpr BufferRange EntireBuffer = BufferRange(0, std::numeric_limits<uint64_t>::max());

    //

    struct MipLevel
    {
        Mip_t Offset = 0;
        Mip_t Count  = 0;

        constexpr MipLevel(
            Mip_t Offset = 0,
            Mip_t Count  = 0) :
            Offset(Offset),
            Count(Count)
        {
        }

        auto operator<=>(
            const MipLevel& Other) const noexcept = default;
    };

    static constexpr MipLevel EntireMipChain = MipLevel(0, std::numeric_limits<Mip_t>::max());

    //

    struct ArraySlice
    {
        Dim_t Offset;
        Dim_t Count;

        constexpr ArraySlice(
            Dim_t Offset = 0,
            Dim_t Count  = 0) :
            Offset(Offset),
            Count(Count)
        {
        }

        auto operator<=>(
            const ArraySlice& Other) const noexcept = default;
    };

    static constexpr ArraySlice EntireArray = ArraySlice(0, std::numeric_limits<Dim_t>::max());

    //

    struct TextureSubresource
    {
        MipLevel   Mips;
        ArraySlice Array;

        constexpr TextureSubresource(
            MipLevel   Mips  = EntireMipChain,
            ArraySlice Array = EntireArray) :
            Mips(Mips),
            Array(Array)
        {
        }

        auto operator<=>(
            const TextureSubresource& Other) const noexcept = default;
    };

    static constexpr TextureSubresource AllSubresources = TextureSubresource(EntireMipChain, EntireArray);

    //

    using BufferDesc  = nri::BufferDesc;
    using TextureDesc = nri::TextureDesc;

    static TextureDesc Tex1D(
        ResourceFormat   Format,
        Dim_t            Width,
        Mip_t            MipNum,
        Dim_t            ArraySize = 1,
        TextureUsageBits UsageMask = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleNum = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_1D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = 1,
            .depth     = 1,
            .mipNum    = MipNum,
            .arraySize = ArraySize,
            .sampleNum = SampleNum
        };
    }

    [[nodiscard]] static TextureDesc Tex2D(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        Mip_t            MipNum,
        Dim_t            ArraySize = 1,
        TextureUsageBits UsageMask = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleNum = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_2D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = Height,
            .depth     = 1,
            .mipNum    = MipNum,
            .arraySize = ArraySize,
            .sampleNum = SampleNum
        };
    }

    [[nodiscard]] static TextureDesc Tex3D(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        uint16_t         Depth,
        Mip_t            MipNum,
        TextureUsageBits UsageMask = TextureUsageBits::SHADER_RESOURCE)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_3D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = Height,
            .depth     = Depth,
            .mipNum    = MipNum,
            .arraySize = 1,
            .sampleNum = 1
        };
    }

    [[nodiscard]] static TextureDesc TexCube(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        Mip_t            MipNum,
        TextureUsageBits UsageMask = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleNum = 1)
    {
        return Tex2D(Format, Width, Height, MipNum, 6, UsageMask, SampleNum);
    }

    //

    struct BufferViewDesc
    {
        BufferRange    Range;
        ResourceFormat Format = ResourceFormat::UNKNOWN;
    };

    struct TextureViewDesc
    {
        TextureViewType    Type;
        TextureSubresource Subresource;
        ResourceFormat     Format = ResourceFormat::UNKNOWN; // default to the format of the texture
        TextureViewFlags   Flags  = TextureViewFlags::None;
    };
} // namespace Ame::Rhi