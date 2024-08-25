#pragma once

#include <Rhi/Core.hpp>
#include <Math/Common.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

namespace Ame::Rhi
{
    // {0BF00221-593F-40CE-B5BD-E47039D77F4A}
    static constexpr UId IID_BufferInitData{ 0xbf00221, 0x593f, 0x40ce, { 0xb5, 0xbd, 0xe4, 0x70, 0x39, 0xd7, 0x7f, 0x4a } };

    // {F8BAB672-F3B8-440C-8709-A05608CFEB23}
    static constexpr UId IID_TextureInitData{ 0xf8bab672, 0xf3b8, 0x440c, { 0x87, 0x9, 0xa0, 0x56, 0x8, 0xcf, 0xeb, 0x23 } };

    struct BufferInitData : BaseObject<IObject>
    {
        BufferInitData(IReferenceCounters* counters) :
            BaseObject(counters)
        {
        }

        static Ptr<BufferInitData> Create()
        {
            return Ptr<BufferInitData>{ ObjectAllocator<BufferInitData>()() };
        }

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_BufferInitData, BaseObject)

        std::vector<uint8_t> Data;

    public:
        [[nodiscard]] Dg::BufferData GetInitData(
            Dg::IDeviceContext* deviceContext = nullptr) const
        {
            return {
                Data.empty() ? nullptr : Data.data(),
                static_cast<uint32_t>(Data.size()),
                deviceContext
            };
        }
    };

    struct TextureInitData : public BaseObject<IObject>
    {
        TextureInitData(IReferenceCounters* counters, Dg::TEXTURE_FORMAT format) :
            BaseObject(counters),
            Format(format)
        {
        }

        const Dg::TEXTURE_FORMAT Format;

        struct LevelData
        {
            std::vector<unsigned char> Data;

            Dg::TextureSubResData SubResData;

            uint32_t Width  = 0;
            uint32_t Height = 0;
        };
        std::vector<LevelData> Levels;

        struct TextureData : Dg::TextureData
        {
            std::vector<Dg::TextureSubResData> Subresources;
        };

        Ptr<Dg::ITexture> StagingTexture;

    public:
        void GenerateMipLevels(
            uint32_t firstMip)
        {
            VERIFY_EXPR(firstMip > 0);
            VERIFY_EXPR(Format != TEX_FORMAT_UNKNOWN);

            const auto& formatAttributes = GetTextureFormatAttribs(Format);

            // Note: this will work even when NumMipLevels is greater than
            //       finest mip resolution. All coarser mip levels will be 1x1.
            for (uint32_t mip = firstMip; mip < Levels.size(); ++mip)
            {
                auto&       level     = Levels[mip];
                const auto& prevLevel = Levels[mip - 1];

                // Note that we can't use GetMipLevelProperties here
                level.Width  = Math::AlignUp(std::max(prevLevel.Width / 2u, 1u), static_cast<uint32_t>(formatAttributes.BlockWidth));
                level.Height = Math::AlignUp(std::max(prevLevel.Height / 2u, 1u), static_cast<uint32_t>(formatAttributes.BlockHeight));

                level.SubResData.Stride =
                    static_cast<uint32_t>(level.Width) / static_cast<uint32_t>(formatAttributes.BlockWidth) * formatAttributes.ComponentSize *
                    (formatAttributes.ComponentType != Dg::COMPONENT_TYPE_COMPRESSED ? formatAttributes.NumComponents : 1);
                level.SubResData.Stride = Math::AlignUp(level.SubResData.Stride, 4ull);

                const auto MipSize = level.SubResData.Stride * level.Height / formatAttributes.BlockHeight;

                level.Data.resize(static_cast<size_t>(MipSize));
                level.SubResData.pData = level.Data.data();

                if (formatAttributes.ComponentType != Dg::COMPONENT_TYPE_COMPRESSED)
                {
                    Dg::ComputeMipLevel({ Format, prevLevel.Width, prevLevel.Height,
                                          prevLevel.Data.data(), static_cast<size_t>(prevLevel.SubResData.Stride),
                                          level.Data.data(), static_cast<size_t>(level.SubResData.Stride) });
                }
                else
                {
                    UNSUPPORTED("Mip generation for compressed formats is not currently implemented");
                }
            }
        }

        [[nodiscard]] TextureData GetInitData(
            Dg::IDeviceContext* deviceContext = nullptr) const
        {
            TextureData initData;
            initData.Subresources = Levels |
                                    std::views::transform([](const LevelData& level)
                                                          { return level.SubResData; }) |
                                    std::ranges::to<std::vector>();
            initData.NumSubresources = static_cast<uint32_t>(initData.Subresources.size());
            initData.pSubResources   = initData.Subresources.data();
            initData.pContext        = deviceContext;
            return initData;
        }
    };
} // namespace Ame::Rhi