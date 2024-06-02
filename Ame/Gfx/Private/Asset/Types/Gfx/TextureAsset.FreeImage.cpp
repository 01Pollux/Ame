#include <Asset/Types/Gfx/TextureAsset.FreeImage.hpp>

namespace Ame::Asset::Gfx
{
    ImageFormat FreeImageToImageFormat(
        FREE_IMAGE_FORMAT format)
    {
        switch (format)
        {
        case FIF_UNKNOWN:
            return ImageFormat::Unknown;
        case FIF_BMP:
            return ImageFormat::Bmp;
        case FIF_ICO:
            return ImageFormat::Ico;
        case FIF_JPEG:
            return ImageFormat::Jpeg;
        case FIF_JNG:
            return ImageFormat::Jng;
        case FIF_KOALA:
            return ImageFormat::Koala;
        case FIF_LBM:
            return ImageFormat::Lbm;
        case FIF_MNG:
            return ImageFormat::Mng;
        case FIF_PBM:
            return ImageFormat::Pbm;
        case FIF_PBMRAW:
            return ImageFormat::PbmRaw;
        case FIF_PCD:
            return ImageFormat::Pcd;
        case FIF_PCX:
            return ImageFormat::Pcx;
        case FIF_PGM:
            return ImageFormat::Pgm;
        case FIF_PGMRAW:
            return ImageFormat::PgmRaw;
        case FIF_PNG:
            return ImageFormat::Png;
        case FIF_PPM:
            return ImageFormat::Ppm;
        case FIF_PPMRAW:
            return ImageFormat::PpmRaw;
        case FIF_RAS:
            return ImageFormat::Ras;
        case FIF_TARGA:
            return ImageFormat::Targa;
        case FIF_TIFF:
            return ImageFormat::Tiff;
        case FIF_WBMP:
            return ImageFormat::Wbmp;
        case FIF_PSD:
            return ImageFormat::Psd;
        case FIF_CUT:
            return ImageFormat::Cut;
        case FIF_XBM:
            return ImageFormat::Xbm;
        case FIF_XPM:
            return ImageFormat::Xpm;
        case FIF_DDS:
            return ImageFormat::Dds;
        case FIF_GIF:
            return ImageFormat::Gif;
        case FIF_HDR:
            return ImageFormat::Hdr;
        case FIF_FAXG3:
            return ImageFormat::FaxG3;
        case FIF_SGI:
            return ImageFormat::Sgi;
        case FIF_EXR:
            return ImageFormat::Exr;
        case FIF_J2K:
            return ImageFormat::J2k;
        case FIF_JP2:
            return ImageFormat::Jp2;
        case FIF_PFM:
            return ImageFormat::Pfm;
        case FIF_PICT:
            return ImageFormat::Pict;
        case FIF_RAW:
            return ImageFormat::Raw;
        case FIF_WEBP:
            return ImageFormat::Webp;
        case FIF_JXR:
            return ImageFormat::Jxr;
        default:
            std::unreachable();
        }
    }
} // namespace Ame::Asset::Gfx