#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Core/hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::BufferDesc>
    {
        size_t operator()(
            const Ame::Rhi::BufferDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.size);
            Ame::HashCombine(hash, desc.structureStride);
            Ame::HashCombine(hash, std::to_underlying(desc.usageMask));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureDesc>
    {
        size_t operator()(
            const Ame::Rhi::TextureDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.type));
            Ame::HashCombine(hash, std::to_underlying(desc.usageMask));
            Ame::HashCombine(hash, std::to_underlying(desc.format));
            Ame::HashCombine(hash, desc.width);
            Ame::HashCombine(hash, desc.height);
            Ame::HashCombine(hash, desc.depth);
            Ame::HashCombine(hash, desc.mipNum);
            Ame::HashCombine(hash, desc.arraySize);
            Ame::HashCombine(hash, desc.sampleNum);

            return hash;
        }
    };
} // namespace std
