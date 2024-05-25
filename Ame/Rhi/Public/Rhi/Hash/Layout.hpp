#pragma once

#include <Rhi/Descs/Layout.hpp>
#include <Core/hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::DescriptorRangeDesc>
    {
        size_t operator()(
            const Ame::Rhi::DescriptorRangeDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.baseRegisterIndex);
            Ame::HashCombine(hash, desc.descriptorNum);
            Ame::HashCombine(hash, std::to_underlying(desc.descriptorType));
            Ame::HashCombine(hash, std::to_underlying(desc.shaderStages));
            Ame::HashCombine(hash, desc.isDescriptorNumVariable);
            Ame::HashCombine(hash, desc.isArray);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DynamicConstantBufferDesc>
    {
        size_t operator()(
            const Ame::Rhi::DynamicConstantBufferDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.registerIndex);
            Ame::HashCombine(hash, std::to_underlying(desc.shaderStages));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::DescriptorSetDesc>
    {
        size_t operator()(
            const Ame::Rhi::DescriptorSetDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.registerSpace);
            for (uint32_t i = 0; i < desc.rangeNum; i++)
            {
                Ame::HashCombine(hash, desc.ranges[i]);
            }
            for (uint32_t i = 0; i < desc.dynamicConstantBufferNum; i++)
            {
                Ame::HashCombine(hash, desc.dynamicConstantBuffers[i]);
            }
            Ame::HashCombine(hash, desc.partiallyBound);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::PushConstantDesc>
    {
        size_t operator()(
            const Ame::Rhi::PushConstantDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.registerIndex);
            Ame::HashCombine(hash, desc.size);
            Ame::HashCombine(hash, std::to_underlying(desc.shaderStages));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::PipelineLayoutDesc>
    {
        size_t operator()(
            const Ame::Rhi::PipelineLayoutDesc& desc) const noexcept
        {
            size_t hash = 0;

            for (uint32_t i = 0; i < desc.descriptorSetNum; i++)
            {
                Ame::HashCombine(hash, desc.descriptorSets[i]);
            }
            for (uint32_t i = 0; i < desc.pushConstantNum; i++)
            {
                Ame::HashCombine(hash, desc.pushConstants[i]);
            }
            Ame::HashCombine(hash, desc.ignoreGlobalSPIRVOffsets);

            return hash;
        }
    };
} // namespace std
