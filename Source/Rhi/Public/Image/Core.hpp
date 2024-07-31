#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame::Rhi
{
    enum class ImageFormat : int8_t
    {
        Unknown = -1,
        Bmp     = 0,
        Ico     = 1,
        Jpeg    = 2,
        Jng     = 3,
        Koala   = 4,
        Lbm     = 5,
        Iff     = Lbm,
        Mng     = 6,
        Pbm     = 7,
        Pbmraw  = 8,
        Pcd     = 9,
        Pcx     = 10,
        Pgm     = 11,
        Pgmraw  = 12,
        Png     = 13,
        Ppm     = 14,
        Ppmraw  = 15,
        Ras     = 16,
        Targa   = 17,
        Tiff    = 18,
        Wbmp    = 19,
        Psd     = 20,
        Cut     = 21,
        Xbm     = 22,
        Xpm     = 23,
        Dds     = 24,
        Gif     = 25,
        Hdr     = 26,
        Faxg3   = 27,
        Sgi     = 28,
        Exr     = 29,
        J2k     = 30,
        Jp2     = 31,
        Pfm     = 32,
        Pict    = 33,
        Raw     = 34,
        Webp    = 35,
        Jxr     = 36
    };

    enum class ImageEncodeFlags : uint16_t
    {
        None = 0,

        // Run-length encoding is a form of lossless data compression in which runs of data are stored as a single
        // occurrence of that data value and a count of its consecutive occurrences, rather than as the original run.
        BitmapRLE = 1 << 0,

        // save data as float instead of as half (not recommended)
        ExrFloat = 1 << 0,
        // save with no compression
        ExrNoCompression = 1 << 1,
        // save with zlib compression, in blocks of 16 scan liness
        ExrZipCompression = 1 << 2,
        // save with piz-based wavelet compression
        ExrPizCompression = 1 << 3,
        // save with lossy 24-bit float compression
        ExrPxr24Compression = 1 << 4,
        // save with lossy 44% float compression - goes to 22% when combined with EXR_LC
        ExrB44Compression = 1 << 5,
        // save images with one luminance and two chroma channels, rather than as RGB (lossy compression)
        ExrLC = 1 << 6,

        // save with superb quality (100:1)
        JpegQualitySuperb = 1 << 0,
        // save with very good quality (75:1)
        JpegQualityGood = 1 << 1,
        // save with good quality (50:1)
        JpegQualityNormal = 1 << 2,
        // save with average quality (25:1)
        JpegQualityAverage = 1 << 3,
        // save with bad quality (10:1)
        JpegQualityBad = 1 << 4,
        // save as a progressive-JPEG (use | to combine with other save flags)
        JpegProgressive = 1 << 5,
        // save with high 4x1 chroma subsampling (4:1:1)
        JpegSubsampling411 = 1 << 6,
        // save with medium 2x2 medium chroma subsampling (4:2:0) - default value
        JpegSubsampling420 = 1 << 7,
        // save with low 2x1 chroma subsampling (4:2:2)
        JpegSubsampling422 = 1 << 8,
        // save with no chroma subsampling (4:4:4)
        JpegSubsampling444 = 1 << 9,
        // on saving, compute optimal Huffman coding tables (can reduce a few percent of file size)
        JpegOptimize = 1 << 10,
        // save basic JPEG, without metadata or any markers
        JpegBaseline = 1 << 11,

        // save using ZLib level 1 compression flag (default value is 6)
        PngBestSpeed = 1 << 0,
        // save using ZLib level 6 compression flag (default recommended value)
        PngDefault = 1 << 1,
        // save using ZLib level 9 compression flag (default value is 6)
        PngBestCompression = 1 << 2,
        // save without ZLib compression
        PngNoCompression = 1 << 3,
        // save using Adam7 interlacing (use | to combine with other save flags)
        PngInterlaced = 1 << 4,

        // if set the writer saves in ASCII format (i.e. P1, P2 or P3)
        PnmAscii = 1 << 0,

        // save without any compression
        PsdNoCompression = 1 << 0,
        // save using RLE compression
        PsdRleCompression = 1 << 1,
        // save using Adobe Large Document Format (use | to combine with other save flags)
        PsdPsb = 1 << 2,

        // output a half-size color image
        RawHalfSize = 1 << 0,
        // output a FIT_UINT16 raw Bayer image
        RawUnprocessed = 1 << 1,

        // if set, the writer saves with RLE compression
        TargaRLE = 1 << 0,

        // stores tags for separated CMYK (use | to combine with compression flags)
        TiffCmyk = 1 << 0,
        // save using PACKBITS compression
        TiffPackBits = 1 << 1,
        // save using DEFLATE compression (a.k.a. ZLIB compression) - obsolete, will save as TIFF_ADOBE_DEFLATE
        TiffDeflate = 1 << 2,
        // save using ADOBE DEFLATE compression
        TiffAdobeDeflate = 1 << 3,
        // save without any compression
        TiffNone = 1 << 4,
        // save using CCITT Group 3 fax encoding
        TiffCCITTFax3 = 1 << 5,
        // save using CCITT Group 4 fax encoding
        TiffCCITTFax4 = 1 << 6,
        // save using LZW compression
        TiffLZW = 1 << 7,
        // save using JPEG compression
        TiffJpeg = 1 << 8,
        // save using LogLuv compression
        TiffLogLuv = 1 << 9,

        // save in lossless mode
        WebpLossless = 1 << 0,

        // save lossless
        JxrLossless = 1 << 0,
        // save as a progressive-JXR (use | to combine with other save flags)
        JxrProgressive = 1 << 1
    };

    enum class ImageDecodeFlags : uint8_t
    {
        None,

        HeaderOnly = 1 << 0,

        // load the image as a 256 color image with ununsed palette entries, if it's 16 or 2 color
        GifLoad256 = 1 << 1,
        // 'Play' the GIF to generate each frame (as 32bpp) instead of returning raw frame data when loading
        GifPlayback = 1 << 2,

        // convert to 32bpp and create an alpha channel from the AND-mask when loading
        IcoMakeAlpha = 1 << 1,

        // load the file as fast as possible, sacrificing some quality
        JpegFast = 1 << 1,
        // load the file with the best quality, sacrificing some speed
        JpegAccurate = 1 << 2,
        // load separated CMYK "as is" (use | to combine with other load flags)
        JpegCmyk = 1 << 3,
        // load and rotate according to Exif 'Orientation' tag if available
        JpegExifRotate = 1 << 4,
        // load and convert to a 8-bit greyscale image
        JpegGreyscale = 1 << 5,

        // load the bitmap sized 768 x 512
        PcdBase = 1 << 1,
        // load the bitmap sized 384 x 256
        PcdBaseDiv4 = 1 << 2,
        // load the bitmap sized 192 x 128
        PcdBaseDiv16 = 1 << 3,

        // loading: avoid gamma correction
        PngIgnoreGamma = 1 << 1,

        // reads tags for separated CMYK (default is conversion to RGB)
        PsdCmyk = 1 << 1,
        // reads tags for CIELab (default is conversion to RGB)
        PsdLab = 1 << 2,

        // try to load the embedded JPEG preview with included Exif Data or default to RGB 24-bit
        RawPreview = 1 << 1,
        // load the file as RGB 24-bit
        RawDisplay = 1 << 2,

        // if set the loader converts RGB555 and ARGB8888 -> RGB888.
        TargaLoadRgb888 = 1 << 1,

        // reads/stores tags for separated CMYK (use | to combine with compression flags)
        TiffCmyk = 1 << 1,
    };

    enum class ImageDataType : uint8_t
    {
        // unknown type
        Unknown = 0,
        // standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
        Bitmap = 1,
        /// <summary>
        // array of unsigned short	: unsigned 16-bit
        Uint16 = 2,
        // array of short			: signed 16-bit
        Int16 = 3,
        // array of unsigned long	: unsigned 32-bit
        Uint32 = 4,
        // array of long			: signed 32-bit
        Int32 = 5,
        // array of float			: 32-bit IEEE floating point
        Float = 6,
        // array of double			: 64-bit IEEE floating point
        Double = 7,
        // array of FICOMPLEX		: 2 x 64-bit IEEE floating point
        Complex = 8,
        // 48-bit RGB image			: 3 x 16-bit
        Rgb16 = 9,
        // 64-bit RGBA image		: 4 x 16-bit
        Rgba16 = 10,
        // 96-bit RGB float image	: 3 x 32-bit IEEE floating point
        Rgbf = 11,
        // 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
        Rgbaf = 12
    };

    enum class ImageFilterType : uint8_t
    {
        // Box, pulse, Fourier window, 1st order (constant) b-spline
        Box = 0,
        // Mitchell & Netravali's two-param cubic filter
        Bicubic = 1,
        // Bilinear filter
        Bilinear = 2,
        // 4th order (cubic) b-spline
        Bspline = 3,
        // Catmull-Rom spline, Overhauser spline
        Catmullrom = 4,
        // Lanczos3 filter
        Lanczos3 = 5
    };

    enum class ImageDitherType : uint8_t
    {
        // Floyd & Steinberg error diffusion
        FS = 0,
        // Bayer ordered dispersed dot dithering (order 2 dithering matrix)
        Bayer4x4 = 1,
        // Bayer ordered dispersed dot dithering (order 3 dithering matrix)
        Bayer8x8 = 2,
        // Ordered clustered dot dithering (order 3 - 6x6 matrix)
        Cluster6x6 = 3,
        // Ordered clustered dot dithering (order 4 - 8x8 matrix)
        Cluster8x8 = 4,
        // Ordered clustered dot dithering (order 8 - 16x16 matrix)
        Cluster16x16 = 5,
        // Bayer ordered dispersed dot dithering (order 4 dithering matrix)
        Bayer16x16 = 6
    };

    class Image;
    class ImageStorge;
} // namespace Ame::Rhi