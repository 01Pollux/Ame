#pragma once

#include <Asset/Asset.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Asset::Gfx
{
    enum class ImageFormat : uint8_t
    {
        Unknown,
        Bmp,
        Ico,
        Jpeg,
        Jng,
        Koala,
        Lbm,
        Iff = Lbm,
        Mng,
        Pbm,
        PbmRaw,
        Pcd,
        Pcx,
        Pgm,
        PgmRaw,
        Png,
        Ppm,
        PpmRaw,
        Ras,
        Targa,
        Tiff,
        Wbmp,
        Psd,
        Cut,
        Xbm,
        Xpm,
        Dds,
        Gif,
        Hdr,
        FaxG3,
        Sgi,
        Exr,
        J2k,
        Jp2,
        Pfm,
        Pict,
        Raw,
        Webp,
        Jxr
    };

    //

    class TextureAsset : public IAsset
    {
    public:
        class Handler;

    public:
        TextureAsset(
            const Ptr<Rhi::ScopedTexture>& texture,
            ImageFormat                    format,
            const Guid&                    guid,
            String                         path);

        /// <summary>
        /// Get underlying texture
        /// </summary>
        [[nodiscard]] const Ptr<Rhi::ScopedTexture>& GetTexture() const;

        /// <summary>
        /// Get texture's format
        /// </summary>
        [[nodiscard]] ImageFormat GetFormat() const;

    private:
        Ptr<Rhi::ScopedTexture> m_Texture;
        String                  m_ShaderSource;
        ImageFormat             m_Format;
    };
} // namespace Ame::Asset::Gfx