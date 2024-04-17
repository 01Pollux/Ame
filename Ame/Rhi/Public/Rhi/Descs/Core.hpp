#pragma once

#include <span>

#include <Core/Enum.hpp>
#include <Math/Vector.hpp>
#include <Math/Colors.hpp>

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    using PipelineLayoutDesc = nri::PipelineLayoutDesc;
    struct GraphicsPipelineDesc;
    struct ComputePipelineDesc;

    using ResourceFormat = nri::Format;
    using ShaderBits     = nri::StageBits;
    using ShaderType     = nri::StageBits;
    using ShaderDesc     = nri::ShaderDesc;
    using MemoryLocation = nri::MemoryLocation;

    using BufferDesc  = nri::BufferDesc;
    using TextureDesc = nri::TextureDesc;

    using TextureRegionDesc     = nri::TextureRegionDesc;
    using TextureDataLayoutDesc = nri::TextureDataLayoutDesc;

    using DescriptorRangeUpdateDesc = nri::DescriptorRangeUpdateDesc;
    using DescriptorSetCopyDesc     = nri::DescriptorSetCopyDesc;

    struct BufferViewDesc;
    struct TextureViewDesc;
} // namespace Ame::Rhi
