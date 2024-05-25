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

    using ResourceFormat        = nri::Format;
    using AttachmentContentType = nri::AttachmentContentType;

    using ShaderType = nri::StageBits;
    using StageBits  = nri::StageBits;
    using ShaderDesc = nri::ShaderDesc;

    static constexpr ShaderType LibraryShaderType = ShaderType::NONE;

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
        StageBits Flags = StageBits::NONE;

        void Reset()
        {
            Flags = StageBits::NONE;
        }

        void Set(
            ShaderFlags other,
            bool        value = true)
        {
            Set(other.Flags, value);
        }

        void Set(
            StageBits flags,
            bool      value = true)
        {
            if (value)
            {
                if (flags == Rhi::ShaderType::ALL || Flags == Rhi::ShaderType::NONE)
                {
                    Flags = flags;
                }
                else if (Flags != Rhi::ShaderType::ALL)
                {
                    Flags |= flags;
                }
            }
            else
            {
                if (flags == Rhi::ShaderType::ALL)
                {
                    Flags = Rhi::ShaderType::NONE;
                }
                else if (Flags != Rhi::ShaderType::NONE)
                {
                    Flags = static_cast<Rhi::ShaderType>(static_cast<uint32_t>(Flags) & ~static_cast<uint32_t>(flags));
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
