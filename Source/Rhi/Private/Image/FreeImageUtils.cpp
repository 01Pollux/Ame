#include <Image/FreeImageUtils.hpp>
#include <mutex>

#include <Core/Enum.hpp>

namespace Ame::Rhi
{
    void FreeImageUtils::Initialize()
    {
        static std::once_flag s_OnceFlag;
        std::call_once(s_OnceFlag,
                       []
                       {
                           FreeImage_Initialise();
                       });
    }

    static unsigned FIReadPorc(
        void*     buffer,
        unsigned  size,
        unsigned  count,
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::istream*>(handle);
        auto  pos    = stream.tellg();
        stream.read(std::bit_cast<char*>(buffer), size * count);
        return static_cast<unsigned>(stream.tellg() - pos);
    }

    static unsigned FIWriteProc(
        void*     buffer,
        unsigned  size,
        unsigned  count,
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        auto  pos    = stream.tellp();
        stream.write(std::bit_cast<char*>(buffer), size * count);
        return static_cast<unsigned>(stream.tellp() - pos);
    }

    static int FISetProc(
        fi_handle handle,
        long      offset,
        int       pos)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        switch (pos)
        {
        case SEEK_SET:
        {
            stream.seekp(offset, std::ios::beg);
            stream.seekg(offset, std::ios::beg);
            break;
        }
        case SEEK_CUR:
        {
            stream.seekp(offset, std::ios::cur);
            stream.seekg(offset, std::ios::cur);
            break;
        }
        case SEEK_END:
        {
            stream.seekp(offset, std::ios::end);
            stream.seekg(offset, std::ios::end);
            break;
        }
        default:
            return 1;
        }
        return 0;
    }

    static long FIGetProc(
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        return static_cast<long>(stream.tellg());
    }

    //

    FreeImageIO FreeImageUtils::GetIO() noexcept
    {
        return FreeImageIO{
            .read_proc  = FIReadPorc,
            .write_proc = FIWriteProc,
            .seek_proc  = FISetProc,
            .tell_proc  = FIGetProc
        };
    }

    //

    static_assert(std::to_underlying(ImageFormat::Unknown) == FIF_UNKNOWN);
    static_assert(std::to_underlying(ImageFormat::Bmp) == FIF_BMP);
    static_assert(std::to_underlying(ImageFormat::Ico) == FIF_ICO);
    static_assert(std::to_underlying(ImageFormat::Jpeg) == FIF_JPEG);
    static_assert(std::to_underlying(ImageFormat::Jng) == FIF_JNG);
    static_assert(std::to_underlying(ImageFormat::Koala) == FIF_KOALA);
    static_assert(std::to_underlying(ImageFormat::Lbm) == FIF_LBM);
    static_assert(std::to_underlying(ImageFormat::Iff) == FIF_IFF);
    static_assert(std::to_underlying(ImageFormat::Mng) == FIF_MNG);
    static_assert(std::to_underlying(ImageFormat::Pbm) == FIF_PBM);
    static_assert(std::to_underlying(ImageFormat::Pbmraw) == FIF_PBMRAW);
    static_assert(std::to_underlying(ImageFormat::Pcd) == FIF_PCD);
    static_assert(std::to_underlying(ImageFormat::Pcx) == FIF_PCX);
    static_assert(std::to_underlying(ImageFormat::Pgm) == FIF_PGM);
    static_assert(std::to_underlying(ImageFormat::Pgmraw) == FIF_PGMRAW);
    static_assert(std::to_underlying(ImageFormat::Png) == FIF_PNG);
    static_assert(std::to_underlying(ImageFormat::Ppm) == FIF_PPM);
    static_assert(std::to_underlying(ImageFormat::Ppmraw) == FIF_PPMRAW);
    static_assert(std::to_underlying(ImageFormat::Ras) == FIF_RAS);
    static_assert(std::to_underlying(ImageFormat::Targa) == FIF_TARGA);
    static_assert(std::to_underlying(ImageFormat::Tiff) == FIF_TIFF);
    static_assert(std::to_underlying(ImageFormat::Wbmp) == FIF_WBMP);
    static_assert(std::to_underlying(ImageFormat::Psd) == FIF_PSD);
    static_assert(std::to_underlying(ImageFormat::Cut) == FIF_CUT);
    static_assert(std::to_underlying(ImageFormat::Xbm) == FIF_XBM);
    static_assert(std::to_underlying(ImageFormat::Xpm) == FIF_XPM);
    static_assert(std::to_underlying(ImageFormat::Dds) == FIF_DDS);
    static_assert(std::to_underlying(ImageFormat::Gif) == FIF_GIF);
    static_assert(std::to_underlying(ImageFormat::Hdr) == FIF_HDR);
    static_assert(std::to_underlying(ImageFormat::Faxg3) == FIF_FAXG3);
    static_assert(std::to_underlying(ImageFormat::Sgi) == FIF_SGI);
    static_assert(std::to_underlying(ImageFormat::Exr) == FIF_EXR);
    static_assert(std::to_underlying(ImageFormat::J2k) == FIF_J2K);
    static_assert(std::to_underlying(ImageFormat::Jp2) == FIF_JP2);
    static_assert(std::to_underlying(ImageFormat::Pfm) == FIF_PFM);
    static_assert(std::to_underlying(ImageFormat::Pict) == FIF_PICT);
    static_assert(std::to_underlying(ImageFormat::Raw) == FIF_RAW);
    static_assert(std::to_underlying(ImageFormat::Webp) == FIF_WEBP);
    static_assert(std::to_underlying(ImageFormat::Jxr) == FIF_JXR);

    FREE_IMAGE_FORMAT FreeImageUtils::ConvertFormat(
        ImageFormat format)
    {
        return static_cast<FREE_IMAGE_FORMAT>(format);
    }

    ImageFormat FreeImageUtils::ConvertFormat(
        FREE_IMAGE_FORMAT format)
    {
        return static_cast<ImageFormat>(format);
    }

    //

    static_assert(std::to_underlying(ImageDataType::Unknown) == FIT_UNKNOWN);
    static_assert(std::to_underlying(ImageDataType::Bitmap) == FIT_BITMAP);
    static_assert(std::to_underlying(ImageDataType::Uint16) == FIT_UINT16);
    static_assert(std::to_underlying(ImageDataType::Int16) == FIT_INT16);
    static_assert(std::to_underlying(ImageDataType::Uint32) == FIT_UINT32);
    static_assert(std::to_underlying(ImageDataType::Int32) == FIT_INT32);
    static_assert(std::to_underlying(ImageDataType::Float) == FIT_FLOAT);
    static_assert(std::to_underlying(ImageDataType::Double) == FIT_DOUBLE);
    static_assert(std::to_underlying(ImageDataType::Complex) == FIT_COMPLEX);
    static_assert(std::to_underlying(ImageDataType::Rgb16) == FIT_RGB16);
    static_assert(std::to_underlying(ImageDataType::Rgba16) == FIT_RGBA16);
    static_assert(std::to_underlying(ImageDataType::Rgbf) == FIT_RGBF);
    static_assert(std::to_underlying(ImageDataType::Rgbaf) == FIT_RGBAF);

    FREE_IMAGE_TYPE FreeImageUtils::ConvertType(ImageDataType type)
    {
        return static_cast<FREE_IMAGE_TYPE>(type);
    }

    static_assert(std::to_underlying(ImageFilterType::Box) == FILTER_BOX);
    static_assert(std::to_underlying(ImageFilterType::Bicubic) == FILTER_BICUBIC);
    static_assert(std::to_underlying(ImageFilterType::Bilinear) == FILTER_BILINEAR);
    static_assert(std::to_underlying(ImageFilterType::Bspline) == FILTER_BSPLINE);
    static_assert(std::to_underlying(ImageFilterType::Catmullrom) == FILTER_CATMULLROM);
    static_assert(std::to_underlying(ImageFilterType::Lanczos3) == FILTER_LANCZOS3);

    FREE_IMAGE_FILTER FreeImageUtils::ConvertFilter(ImageFilterType filter)
    {
        return static_cast<FREE_IMAGE_FILTER>(filter);
    }

    //

    int FreeImageUtils::ConvertFlags(
        ImageEncodeFlags flags)
    {
        if (flags == ImageEncodeFlags::None)
        {
            return 0;
        }

        constexpr std::array s_Flags{
            std::pair{ ImageEncodeFlags::BitmapRLE, BMP_SAVE_RLE },
            std::pair{ ImageEncodeFlags::ExrFloat, EXR_FLOAT },
            std::pair{ ImageEncodeFlags::ExrNoCompression, EXR_NONE },
            std::pair{ ImageEncodeFlags::ExrZipCompression, EXR_ZIP },
            std::pair{ ImageEncodeFlags::ExrPizCompression, EXR_PIZ },
            std::pair{ ImageEncodeFlags::ExrPxr24Compression, EXR_PXR24 },
            std::pair{ ImageEncodeFlags::ExrB44Compression, EXR_B44 },
            std::pair{ ImageEncodeFlags::ExrLC, EXR_LC },
            std::pair{ ImageEncodeFlags::JpegQualitySuperb, JPEG_QUALITYSUPERB },
            std::pair{ ImageEncodeFlags::JpegQualityGood, JPEG_QUALITYGOOD },
            std::pair{ ImageEncodeFlags::JpegQualityNormal, JPEG_QUALITYNORMAL },
            std::pair{ ImageEncodeFlags::JpegQualityAverage, JPEG_QUALITYAVERAGE },
            std::pair{ ImageEncodeFlags::JpegQualityBad, JPEG_QUALITYBAD },
            std::pair{ ImageEncodeFlags::JpegProgressive, JPEG_PROGRESSIVE },
            std::pair{ ImageEncodeFlags::JpegSubsampling411, JPEG_SUBSAMPLING_411 },
            std::pair{ ImageEncodeFlags::JpegSubsampling420, JPEG_SUBSAMPLING_420 },
            std::pair{ ImageEncodeFlags::JpegSubsampling422, JPEG_SUBSAMPLING_422 },
            std::pair{ ImageEncodeFlags::JpegSubsampling444, JPEG_SUBSAMPLING_444 },
            std::pair{ ImageEncodeFlags::JpegOptimize, JPEG_OPTIMIZE },
            std::pair{ ImageEncodeFlags::JpegBaseline, JPEG_BASELINE },
            std::pair{ ImageEncodeFlags::PngBestSpeed, PNG_Z_BEST_SPEED },
            std::pair{ ImageEncodeFlags::PngDefault, PNG_Z_DEFAULT_COMPRESSION },
            std::pair{ ImageEncodeFlags::PngBestCompression, PNG_Z_BEST_COMPRESSION },
            std::pair{ ImageEncodeFlags::PngNoCompression, PNG_Z_NO_COMPRESSION },
            std::pair{ ImageEncodeFlags::PngInterlaced, PNG_INTERLACED },
            std::pair{ ImageEncodeFlags::PnmAscii, PNM_SAVE_ASCII },
            std::pair{ ImageEncodeFlags::PsdNoCompression, PSD_NONE },
            std::pair{ ImageEncodeFlags::PsdRleCompression, PSD_RLE },
            std::pair{ ImageEncodeFlags::PsdPsb, PSD_PSB },
            std::pair{ ImageEncodeFlags::RawHalfSize, RAW_HALFSIZE },
            std::pair{ ImageEncodeFlags::RawUnprocessed, RAW_UNPROCESSED },
            std::pair{ ImageEncodeFlags::TargaRLE, TARGA_SAVE_RLE },
            std::pair{ ImageEncodeFlags::TiffCmyk, TIFF_CMYK },
            std::pair{ ImageEncodeFlags::TiffPackBits, TIFF_PACKBITS },
            std::pair{ ImageEncodeFlags::TiffDeflate, TIFF_DEFLATE },
            std::pair{ ImageEncodeFlags::TiffAdobeDeflate, TIFF_ADOBE_DEFLATE },
            std::pair{ ImageEncodeFlags::TiffNone, TIFF_NONE },
            std::pair{ ImageEncodeFlags::TiffCCITTFax3, TIFF_CCITTFAX3 },
            std::pair{ ImageEncodeFlags::TiffCCITTFax4, TIFF_CCITTFAX4 },
            std::pair{ ImageEncodeFlags::TiffLZW, TIFF_LZW },
            std::pair{ ImageEncodeFlags::TiffJpeg, TIFF_JPEG },
            std::pair{ ImageEncodeFlags::TiffLogLuv, TIFF_LOGLUV },
            std::pair{ ImageEncodeFlags::WebpLossless, WEBP_LOSSLESS },
            std::pair{ ImageEncodeFlags::JxrLossless, JXR_LOSSLESS },
            std::pair{ ImageEncodeFlags::JxrProgressive, JXR_PROGRESSIVE },
        };

        using namespace EnumBitOperators;

        int result = 0;
        for (auto [flag, value] : s_Flags)
        {
            if ((flags & flag) != ImageEncodeFlags::None)
            {
                result |= value;
            }
        }
        return result;
    }

    int FreeImageUtils::ConvertFlags(
        ImageDecodeFlags flags)
    {
        if (flags == ImageDecodeFlags::None)
        {
            return 0;
        }

        constexpr std::array s_Flags{
            std::pair{ ImageDecodeFlags::HeaderOnly, FIF_LOAD_NOPIXELS },
            std::pair{ ImageDecodeFlags::GifLoad256, GIF_LOAD256 },
            std::pair{ ImageDecodeFlags::GifPlayback, GIF_PLAYBACK },
            std::pair{ ImageDecodeFlags::IcoMakeAlpha, ICO_MAKEALPHA },
            std::pair{ ImageDecodeFlags::JpegFast, JPEG_FAST },
            std::pair{ ImageDecodeFlags::JpegAccurate, JPEG_ACCURATE },
            std::pair{ ImageDecodeFlags::JpegCmyk, JPEG_CMYK },
            std::pair{ ImageDecodeFlags::JpegExifRotate, JPEG_EXIFROTATE },
            std::pair{ ImageDecodeFlags::JpegGreyscale, JPEG_GREYSCALE },
            std::pair{ ImageDecodeFlags::PcdBase, PCD_BASE },
            std::pair{ ImageDecodeFlags::PcdBaseDiv4, PCD_BASEDIV4 },
            std::pair{ ImageDecodeFlags::PcdBaseDiv16, PCD_BASEDIV16 },
            std::pair{ ImageDecodeFlags::PngIgnoreGamma, PNG_IGNOREGAMMA },
            std::pair{ ImageDecodeFlags::PsdCmyk, PSD_CMYK },
            std::pair{ ImageDecodeFlags::PsdLab, PSD_LAB },
            std::pair{ ImageDecodeFlags::RawPreview, RAW_PREVIEW },
            std::pair{ ImageDecodeFlags::RawDisplay, RAW_DISPLAY },
            std::pair{ ImageDecodeFlags::TargaLoadRgb888, TARGA_LOAD_RGB888 },
            std::pair{ ImageDecodeFlags::TiffCmyk, TIFF_CMYK },

        };

        using namespace EnumBitOperators;

        int result = 0;
        for (auto [flag, value] : s_Flags)
        {
            if ((flags & flag) != ImageDecodeFlags::None)
            {
                result |= value;
            }
        }
        return result;
    }

    FIBITMAP* FreeImageUtils::GetBitmap(
        void* bitmap)
    {
        return static_cast<FIBITMAP*>(bitmap);
    }

    FIMEMORY* FreeImageUtils::GetMemory(void* bitmap)
    {
        return static_cast<FIMEMORY*>(bitmap);
    }

    FIMULTIBITMAP* FreeImageUtils::GetBitmapArray(
        void* bitmap)
    {
        return static_cast<FIMULTIBITMAP*>(bitmap);
    }
} // namespace Ame::Rhi