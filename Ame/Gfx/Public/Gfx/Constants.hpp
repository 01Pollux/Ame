#pragma once

#include <Gfx/Shading/Constants.hpp>

namespace Ame::Gfx::Constants::DescriptorRanges
{
    static constexpr uint32_t FrameData_RegisterIndex = 0;
    static constexpr uint32_t FrameData_SetIndex      = 0;
    static constexpr uint32_t FrameData_RegisterSpace = 1;

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorRangeDesc FrameRangeDesc{
        .baseRegisterIndex = FrameData_RegisterIndex,
        .descriptorNum     = 1,
        .descriptorType    = Rhi::DescriptorType::CONSTANT_BUFFER,
        .shaderStages      = ShaderTypes
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorSetDesc FrameSetDesc{
        .registerSpace = FrameData_RegisterSpace,
        .ranges        = &FrameRangeDesc<ShaderTypes>,
        .rangeNum      = 1
    };

    //

    static constexpr uint32_t EntityData_RegisterIndex = 0;
    static constexpr uint32_t EntityData_SetIndex      = 1;
    static constexpr uint32_t EntityData_RegisterSpace = 2;

    enum class EntityDataTypes : uint8_t
    {
        Transforms,
        RenderInstances,

        Count
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorRangeDesc EntityRangeDesc{
        .baseRegisterIndex = EntityData_RegisterIndex,
        .descriptorNum     = std::to_underlying(EntityDataTypes::Count),
        .descriptorType    = Rhi::DescriptorType::STRUCTURED_BUFFER,
        .shaderStages      = ShaderTypes
    };

    template<typename Rhi::StageBits ShaderTypes = Rhi::StageBits::NONE>
    static constexpr Rhi::DescriptorSetDesc EntitySetDesc{
        .registerSpace = EntityData_RegisterSpace,
        .ranges        = &EntityRangeDesc<ShaderTypes>,
        .rangeNum      = 1
    };

    //

    static constexpr uint32_t MaterialData_SetIndex      = 3;
    static constexpr uint32_t MaterialData_RegisterSpace = 20;
} // namespace Ame::Gfx::Constants::DescriptorRanges
