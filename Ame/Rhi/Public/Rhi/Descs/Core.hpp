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
    using ShaderType     = nri::StageBits;
    using ShaderDesc     = nri::ShaderDesc;
    using MemoryLocation = nri::MemoryLocation;

    using BufferDesc  = nri::BufferDesc;
    using TextureDesc = nri::TextureDesc;

    using SamplerFilter       = nri::Filter;
    using SamplerFilterExt    = nri::FilterExt;
    using SamplerFilters      = nri::Filters;
    using SamplerAddressMode  = nri::AddressMode;
    using SamplerAddressModes = nri::AddressModes;
    using SamplerBorderColor  = nri::BorderColor;
    using SamplerDesc         = nri::SamplerDesc;

    using TextureRegionDesc     = nri::TextureRegionDesc;
    using TextureDataLayoutDesc = nri::TextureDataLayoutDesc;

    using DescriptorRangeUpdateDesc = nri::DescriptorRangeUpdateDesc;
    using DescriptorSetCopyDesc     = nri::DescriptorSetCopyDesc;

    using AccessBits        = nri::AccessBits;
    using AccessStage       = nri::AccessStage;
    using LayoutType        = nri::Layout;
    using AccessLayoutStage = nri::AccessLayoutStage;

    using GlobalBarrierDesc = nri::GlobalBarrierDesc;

    struct BufferViewDesc;
    struct TextureViewDesc;

    //

    struct ShaderFlags
    {
        ShaderType Flags = ShaderType::NONE;

        void Set(
            ShaderFlags ToSet,
            bool        Value = true)
        {
            Set(ToSet.Flags, Value);
        }

        void Set(
            ShaderType ToSet,
            bool       Value = true)
        {
            if (Value)
            {
                if (ToSet == Rhi::ShaderType::ALL || Flags == Rhi::ShaderType::NONE)
                {
                    Flags = ToSet;
                }
                else if (Flags != Rhi::ShaderType::ALL)
                {
                    Flags |= ToSet;
                }
            }
            else
            {
                if (ToSet == Rhi::ShaderType::ALL)
                {
                    Flags = Rhi::ShaderType::NONE;
                }
                else if (Flags != Rhi::ShaderType::NONE)
                {
                    Flags = static_cast<Rhi::ShaderType>(static_cast<uint32_t>(Flags) & ~static_cast<uint32_t>(ToSet));
                }
            }
        }

        [[nodiscard]] static ShaderFlags All()
        {
            return { ShaderType::ALL };
        }

        [[nodiscard]] static ShaderFlags None()
        {
            return { ShaderType::NONE };
        }
    };
} // namespace Ame::Rhi
