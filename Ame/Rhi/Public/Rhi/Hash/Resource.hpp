#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Core/Hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::BufferDesc>
    {
        size_t operator()(
            const Ame::Rhi::BufferDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.size);
            Ame::HashCombine(Hash, Desc.structureStride);
            Ame::HashCombine(Hash, std::to_underlying(Desc.usageMask));

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

            Ame::HashCombine(Hash, std::to_underlying(Desc.type));
            Ame::HashCombine(Hash, std::to_underlying(Desc.usageMask));
            Ame::HashCombine(Hash, std::to_underlying(Desc.format));
            Ame::HashCombine(Hash, Desc.width);
            Ame::HashCombine(Hash, Desc.height);
            Ame::HashCombine(Hash, Desc.depth);
            Ame::HashCombine(Hash, Desc.mipNum);
            Ame::HashCombine(Hash, Desc.arraySize);
            Ame::HashCombine(Hash, Desc.sampleNum);

            return Hash;
        }
    };
} // namespace std
