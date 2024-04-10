#pragma once

#include <span>

#include <NRI/NRI.h>
#include <NRI/NRIDescs.h>

#include <Rhi/Core.hpp>
#include <Core/Enum.hpp>
#include <Math/Vector.hpp>

namespace Ame::Rhi
{
    class Device;
    class PipelineLayout;
    class PipelineState;
    class Buffer;
    class Texture;

    using PipelineLayoutDesc = nri::PipelineLayoutDesc;
    struct GraphicsPipelineDesc;
    struct ComputePipelineDesc;

    using ResourceFormat = nri::Format;
    using ShaderBits     = nri::StageBits;
    using ShaderType     = nri::StageBits;
    using ShaderDesc     = nri::ShaderDesc;

    using BufferDesc  = nri::BufferDesc;
    using TextureDesc = nri::TextureDesc;
    struct BufferViewDesc;
    struct TextureViewDesc;

    using Mip_t    = uint8_t;
    using Dim_t    = uint8_t;
    using Sample_t = uint8_t;
} // namespace Ame::Rhi
