#pragma once

#include <Gfx/Shading/Constants.hpp>

namespace Ame::Gfx::Constants::DescriptorRanges
{
    static constexpr uint32_t c_FrameData_RegisterIndex = 0;
    static constexpr uint32_t c_FrameData_SetIndex      = 0;
    static constexpr uint32_t c_FrameData_RegisterSpace = 1;

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorRangeDesc c_FrameRangeDesc{
        .baseRegisterIndex = c_FrameData_RegisterIndex,
        .descriptorNum     = 1,
        .descriptorType    = Rhi::DescriptorType::CONSTANT_BUFFER,
        .shaderStages      = ShaderTypes
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorSetDesc c_FrameSetDesc{
        .registerSpace = c_FrameData_RegisterSpace,
        .ranges        = &c_FrameRangeDesc<ShaderTypes>,
        .rangeNum      = 1
    };

    //

    static constexpr uint32_t c_EntityData_RegisterIndex = 0;
    static constexpr uint32_t c_EntityData_SetIndex      = 1;
    static constexpr uint32_t c_EntityData_RegisterSpace = 2;

    enum class EntityDataTypes : uint8_t
    {
        Transforms,
        RenderInstances,

        Count
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorRangeDesc c_EntityRangeDesc{
        .baseRegisterIndex = c_EntityData_RegisterIndex,
        .descriptorNum     = std::to_underlying(EntityDataTypes::Count),
        .descriptorType    = Rhi::DescriptorType::STRUCTURED_BUFFER,
        .shaderStages      = ShaderTypes
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorSetDesc c_EntitySetDesc{
        .registerSpace = c_EntityData_RegisterSpace,
        .ranges        = &c_EntityRangeDesc<ShaderTypes>,
        .rangeNum      = 1
    };

    //

    static constexpr uint32_t c_MaterialData_SetIndex      = 2;
    static constexpr uint32_t c_MaterialData_RegisterSpace = 15;
} // namespace Ame::Gfx::Constants::DescriptorRanges
