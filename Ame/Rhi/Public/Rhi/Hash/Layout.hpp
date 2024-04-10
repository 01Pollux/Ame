#pragma once

#include <Rhi/Descs/Layout.hpp>
#include <Core/Hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::DescriptorRangeDesc>
    {
        size_t operator()(
            const Ame::Rhi::DescriptorRangeDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.baseRegisterIndex);
            Ame::HashCombine(Hash, Desc.descriptorNum);
            Ame::HashCombine(Hash, std::to_underlying(Desc.descriptorType));
            Ame::HashCombine(Hash, std::to_underlying(Desc.shaderStages));
            Ame::HashCombine(Hash, Desc.isDescriptorNumVariable);
            Ame::HashCombine(Hash, Desc.isArray);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DynamicConstantBufferDesc>
    {
        size_t operator()(
            const Ame::Rhi::DynamicConstantBufferDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.registerIndex);
            Ame::HashCombine(Hash, std::to_underlying(Desc.shaderStages));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DescriptorSetDesc>
    {
        size_t operator()(
            const Ame::Rhi::DescriptorSetDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.registerSpace);
            for (uint32_t i = 0; i < Desc.rangeNum; i++)
            {
                Ame::HashCombine(Hash, Desc.ranges[i]);
            }
            for (uint32_t i = 0; i < Desc.dynamicConstantBufferNum; i++)
            {
                Ame::HashCombine(Hash, Desc.dynamicConstantBuffers[i]);
            }
            Ame::HashCombine(Hash, Desc.partiallyBound);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::PushConstantDesc>
    {
        size_t operator()(
            const Ame::Rhi::PushConstantDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.registerIndex);
            Ame::HashCombine(Hash, Desc.size);
            Ame::HashCombine(Hash, std::to_underlying(Desc.shaderStages));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::PipelineLayoutDesc>
    {
        size_t operator()(
            const Ame::Rhi::PipelineLayoutDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            for (uint32_t i = 0; i < Desc.descriptorSetNum; i++)
            {
                Ame::HashCombine(Hash, Desc.descriptorSets[i]);
            }
            for (uint32_t i = 0; i < Desc.pushConstantNum; i++)
            {
                Ame::HashCombine(Hash, Desc.pushConstants[i]);
            }
            Ame::HashCombine(Hash, Desc.ignoreGlobalSPIRVOffsets);

            return Hash;
        }
    };
} // namespace std
