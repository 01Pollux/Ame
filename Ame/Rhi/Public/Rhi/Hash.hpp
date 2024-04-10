#pragma once

#include <Rhi/Resource.hpp>
#include <boost/container_hash/hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::DescriptorRangeDesc>
    {
        size_t operator()(
            const Ame::Rhi::DescriptorRangeDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            boost::hash_combine(Hash, Desc.baseRegisterIndex);
            boost::hash_combine(Hash, Desc.descriptorNum);
            boost::hash_combine(Hash, std::to_underlying(Desc.descriptorType));
            boost::hash_combine(Hash, std::to_underlying(Desc.shaderStages));
            boost::hash_combine(Hash, Desc.isDescriptorNumVariable);
            boost::hash_combine(Hash, Desc.isArray);

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

            boost::hash_combine(Hash, Desc.registerIndex);
            boost::hash_combine(Hash, std::to_underlying(Desc.shaderStages));

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

            boost::hash_combine(Hash, Desc.registerSpace);
            for (uint32_t i = 0; i < Desc.rangeNum; i++)
            {
				boost::hash_combine(Hash, Desc.ranges[i]);
            }
            for (uint32_t i = 0; i < Desc.dynamicConstantBufferNum; i++)
            {
				boost::hash_combine(Hash, Desc.dynamicConstantBuffers[i]);
			}
            boost::hash_combine(Hash, Desc.partiallyBound);

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

            boost::hash_combine(Hash, Desc.registerIndex);
            boost::hash_combine(Hash, Desc.size);
            boost::hash_combine(Hash, std::to_underlying(Desc.shaderStages));

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
                boost::hash_combine(Hash, Desc.descriptorSets[i]);
            }
            for (uint32_t i = 0; i < Desc.pushConstantNum; i++)
            {
				boost::hash_combine(Hash, Desc.pushConstants[i]);
            }
            boost::hash_combine(Hash, Desc.ignoreGlobalSPIRVOffsets);

            return Hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::BufferDesc>
    {
        size_t operator()(
            const Ame::Rhi::BufferDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            boost::hash_combine(Hash, Desc.size);
            boost::hash_combine(Hash, Desc.structureStride);
            boost::hash_combine(Hash, std::to_underlying(Desc.usageMask));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureDesc>
    {
        size_t operator()(
            const Ame::Rhi::TextureDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            boost::hash_combine(Hash, std::to_underlying(Desc.type));
            boost::hash_combine(Hash, std::to_underlying(Desc.usageMask));
            boost::hash_combine(Hash, std::to_underlying(Desc.format));
            boost::hash_combine(Hash, Desc.width);
            boost::hash_combine(Hash, Desc.height);
            boost::hash_combine(Hash, Desc.depth);
            boost::hash_combine(Hash, Desc.mipNum);
            boost::hash_combine(Hash, Desc.arraySize);
            boost::hash_combine(Hash, Desc.sampleNum);

            return Hash;
        }
    };
} // namespace std
