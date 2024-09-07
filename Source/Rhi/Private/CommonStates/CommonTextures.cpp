#include <ranges>

#include <CommonStates/CommonRenderPasses/CommonTextures.hpp>
#include <Rhi/Device/RhiDevice.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include <Image/Image.hpp>
#include <Image/ImageStorage.hpp>

#include <CommonStates/Textures.hpp>
#include <Core/Hash.hpp>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>

namespace Ame::Rhi
{
    [[nodiscard]] static std::vector<std::uint8_t> DecompressTextureData(std::span<const std::uint8_t> data)
    {
        namespace bio = boost::iostreams;

        std::vector<std::uint8_t> image;

        bio::filtering_istream stream;
        stream.push(bio::gzip_decompressor());
        stream.push(bio::array_source(std::bit_cast<const char*>(data.data()), data.size()));
        bio::copy(stream, std::back_inserter(image));

        return image;
    }

    [[nodiscard]] static Ptr<Dg::ITexture> CreateTexture(IRhiDevice* rhiDevice, Dg::RESOURCE_DIMENSION dim,
                                                         [[maybe_unused]] const char* name, const Math::Vector3U& size,
                                                         std::span<Dg::TextureSubResData> subresourceData,
                                                         Dg::TEXTURE_FORMAT format = Dg::TEX_FORMAT_RGBA8_UNORM)
    {
        auto renderDevice = rhiDevice->GetRenderDevice();

        Dg::TextureDesc textureDesc{
#ifndef AME_DIST
            name,
#else
            nullptr,
#endif
            dim,  size.x(), size.y(), size.z(), format, 1u, 1u, Dg::USAGE_IMMUTABLE, Dg::BIND_SHADER_RESOURCE
        };

        Dg::TextureData   initData{ subresourceData.data(), Rhi::Count32(subresourceData) };
        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(textureDesc, &initData, &texture);
        return texture;
    }

    //

    CommonTextures::CommonTextures(IRhiDevice* rhiDevice) : m_RhiDevice(rhiDevice)
    {
    }

    //

    Dg::ITexture* CommonTextures::GetDefaultTexture(CommonTexture type)
    {
        auto index = std::to_underlying(type);
        if (!m_DefaultTextures[index])
        {
            switch (type)
            {
#define COMMON_TEXTURES_111(Name)                                                                                      \
    case CommonTexture::Name##1D:                                                                                      \
    {                                                                                                                  \
        Dg::TextureSubResData subresources[] = { { s_##Name##Texture, sizeof(s_##Name##Texture) } };                   \
        m_DefaultTextures[index] =                                                                                     \
            CreateTexture(m_RhiDevice, Dg::RESOURCE_DIM_TEX_1D, #Name "1D", { 1, 1, 1 }, subresources);                \
        break;                                                                                                         \
    }                                                                                                                  \
    case CommonTexture::Name##2D:                                                                                      \
    {                                                                                                                  \
        Dg::TextureSubResData subresources[] = { { s_##Name##Texture, sizeof(s_##Name##Texture) } };                   \
        m_DefaultTextures[index] =                                                                                     \
            CreateTexture(m_RhiDevice, Dg::RESOURCE_DIM_TEX_2D, #Name "2D", { 1, 1, 1 }, subresources);                \
        break;                                                                                                         \
    }                                                                                                                  \
    case CommonTexture::Name##3D:                                                                                      \
    {                                                                                                                  \
        Dg::TextureSubResData subresources[] = { { s_##Name##Texture, sizeof(s_##Name##Texture) } };                   \
        m_DefaultTextures[index] =                                                                                     \
            CreateTexture(m_RhiDevice, Dg::RESOURCE_DIM_TEX_3D, #Name "3D", { 1, 1, 1 }, subresources);                \
        break;                                                                                                         \
    }                                                                                                                  \
    case CommonTexture::Name##Cube:                                                                                    \
    {                                                                                                                  \
        Dg::TextureSubResData subresources[] = { { s_##Name##Texture, sizeof(s_##Name##Texture) } };                   \
        m_DefaultTextures[index] =                                                                                     \
            CreateTexture(m_RhiDevice, Dg::RESOURCE_DIM_TEX_3D, #Name "Cube", { 1, 1, 1 }, subresources);              \
        break;                                                                                                         \
    }

                COMMON_TEXTURES_111(White);
                COMMON_TEXTURES_111(Black);
                COMMON_TEXTURES_111(Grey);

            case CommonTexture::Checkboard:
            {
                m_DefaultTextures[index] = GetCheckboardTexture();
                break;
            }

            case CommonTexture::DevTexture:
            {
                m_DefaultTextures[index] = GetDevTexture();
                break;
            }

            default:
                std::unreachable();
            }
        }
        return m_DefaultTextures[index];
    }

    //

    Dg::ITexture* CommonTextures::GetCheckboardTexture(const Math::Vector2U& size, const Math::Vector2U& slices)
    {
        auto  index   = HashCompute(size.x(), size.y(), slices.x(), slices.y());
        auto& texture = m_CheckboardTextures[index];

        if (!texture)
        {
            size_t               rowPitch = size.x() * sizeof(uint8_t[4]);
            std::vector<uint8_t> data(rowPitch * size.y());
            Dg::GenerateCheckerBoardPattern(
                size.x(), size.y(), Dg::TEX_FORMAT_RGBA8_UNORM, slices.x(), slices.y(), data.data(), rowPitch);

            Dg::TextureSubResData subresources[]{ { data.data(), rowPitch } };
            texture = CreateTexture(
                m_RhiDevice, Dg::RESOURCE_DIM_TEX_2D, "Checkboard", { size.x(), size.y(), 1 }, subresources);
        }
        return texture;
    }

    //

    Dg::ITexture* CommonTextures::GetDevTexture(const Math::Vector2U& size)
    {
        auto  index   = HashCompute(size.x(), size.y());
        auto& texture = m_DevTextures[index];

        if (!texture)
        {
            auto imageData   = DecompressTextureData(s_DevTexture);
            auto imageMemory = ImageStorage::Load(std::bit_cast<std::byte*>(imageData.data()), imageData.size());
            auto image       = imageMemory.GetImage();

            if (image.GetSize() != size)
            {
                image = image.Rescale(size);
            }
            image = image.ConvertTo(ImageDataType::Rgbaf);

            Dg::TextureSubResData subresources[]{ image.GetSubresource() };
            texture = CreateTexture(m_RhiDevice, Dg::RESOURCE_DIM_TEX_2D, "Development", { size.x(), size.y(), 1 },
                                    subresources, Dg::TEX_FORMAT_RGBA32_FLOAT);
        }
        return texture;
    }
} // namespace Ame::Rhi