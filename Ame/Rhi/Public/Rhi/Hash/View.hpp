#pragma once

#include <Rhi/Descs/View.hpp>
#include <Core/Hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::BufferRange>
    {
        size_t operator()(
            const Ame::Rhi::BufferRange& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Offset);
            Ame::HashCombine(Hash, Desc.Size);

            return Hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::MipLevel>
    {
        size_t operator()(
            const Ame::Rhi::MipLevel& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Offset);
            Ame::HashCombine(Hash, Desc.Count);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::ArraySlice>
    {
        size_t operator()(
            const Ame::Rhi::ArraySlice& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Offset);
            Ame::HashCombine(Hash, Desc.Count);

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureSubresource>
    {
        size_t operator()(
            const Ame::Rhi::TextureSubresource& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Mips);
            Ame::HashCombine(Hash, Desc.Array);

            return Hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::BufferViewDesc>
    {
        size_t operator()(
            const Ame::Rhi::BufferViewDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, Desc.Range);
            Ame::HashCombine(Hash, std::to_underlying(Desc.Format));
            Ame::HashCombine(Hash, std::to_underlying(Desc.Type));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureViewDesc>
    {
        size_t operator()(
            const Ame::Rhi::TextureViewDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.Type));
            Ame::HashCombine(Hash, Desc.Subresource);
            Ame::HashCombine(Hash, std::to_underlying(Desc.Format));
            Ame::HashCombine(Hash, std::to_underlying(Desc.Flags));

            return Hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::SamplerDesc>
    {
        size_t operator()(
            const Ame::Rhi::SamplerDesc& Desc) const noexcept
        {
            size_t Hash = 0;

            Ame::HashCombine(Hash, std::to_underlying(Desc.filters.min));
            Ame::HashCombine(Hash, std::to_underlying(Desc.filters.mag));
            Ame::HashCombine(Hash, std::to_underlying(Desc.filters.mip));
            Ame::HashCombine(Hash, std::to_underlying(Desc.filters.ext));
            Ame::HashCombine(Hash, Desc.anisotropy);
            Ame::HashCombine(Hash, Desc.mipBias);
            Ame::HashCombine(Hash, Desc.mipMin);
            Ame::HashCombine(Hash, Desc.mipMax);
            Ame::HashCombine(Hash, std::to_underlying(Desc.addressModes.u));
            Ame::HashCombine(Hash, std::to_underlying(Desc.addressModes.v));
            Ame::HashCombine(Hash, std::to_underlying(Desc.addressModes.w));
            Ame::HashCombine(Hash, std::to_underlying(Desc.compareFunc));
            Ame::HashCombine(Hash, std::to_underlying(Desc.borderColor));

            return Hash;
        }
    };
} // namespace std
