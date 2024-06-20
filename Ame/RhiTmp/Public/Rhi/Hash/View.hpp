#pragma once

#include <Rhi/Descs/View.hpp>
#include <Core/hash.hpp>

namespace std
{
    template<>
    struct hash<Ame::Rhi::BufferRange>
    {
        size_t operator()(
            const Ame::Rhi::BufferRange& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Offset);
            Ame::HashCombine(hash, desc.Size);

            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::MipLevel>
    {
        size_t operator()(
            const Ame::Rhi::MipLevel& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Offset);
            Ame::HashCombine(hash, desc.Count);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::ArraySlice>
    {
        size_t operator()(
            const Ame::Rhi::ArraySlice& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Offset);
            Ame::HashCombine(hash, desc.Count);

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureSubresource>
    {
        size_t operator()(
            const Ame::Rhi::TextureSubresource& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Mips);
            Ame::HashCombine(hash, desc.Array);

            return hash;
        }
    };

    //

    template<>
    struct hash<Ame::Rhi::BufferViewDesc>
    {
        size_t operator()(
            const Ame::Rhi::BufferViewDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, desc.Range);
            Ame::HashCombine(hash, std::to_underlying(desc.Format));
            Ame::HashCombine(hash, std::to_underlying(desc.Type));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::TextureViewDesc>
    {
        size_t operator()(
            const Ame::Rhi::TextureViewDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.Type));
            Ame::HashCombine(hash, desc.Subresource);
            Ame::HashCombine(hash, std::to_underlying(desc.Format));
            Ame::HashCombine(hash, std::to_underlying(desc.Flags));

            return hash;
        }
    };

    template<>
    struct hash<Ame::Rhi::SamplerDesc>
    {
        size_t operator()(
            const Ame::Rhi::SamplerDesc& desc) const noexcept
        {
            size_t hash = 0;

            Ame::HashCombine(hash, std::to_underlying(desc.filters.min));
            Ame::HashCombine(hash, std::to_underlying(desc.filters.mag));
            Ame::HashCombine(hash, std::to_underlying(desc.filters.mip));
            Ame::HashCombine(hash, std::to_underlying(desc.filters.ext));
            Ame::HashCombine(hash, desc.anisotropy);
            Ame::HashCombine(hash, desc.mipBias);
            Ame::HashCombine(hash, desc.mipMin);
            Ame::HashCombine(hash, desc.mipMax);
            Ame::HashCombine(hash, std::to_underlying(desc.addressModes.u));
            Ame::HashCombine(hash, std::to_underlying(desc.addressModes.v));
            Ame::HashCombine(hash, std::to_underlying(desc.addressModes.w));
            Ame::HashCombine(hash, std::to_underlying(desc.compareFunc));
            Ame::HashCombine(hash, std::to_underlying(desc.borderColor));

            return hash;
        }
    };
} // namespace std
