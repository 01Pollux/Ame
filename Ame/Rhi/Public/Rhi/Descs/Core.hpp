#pragma once

#include <span>

#include <Rhi/Core.hpp>
#include <Core/Enum.hpp>
#include <Math/Vector.hpp>
#include <Math/Colors.hpp>

namespace Ame::Rhi
{
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
} // namespace Ame::Rhi
