#include <mutex>
#include <map>

#include <FreeImage/FreeImage.hpp>

namespace Ame::Extensions
{
    static std::mutex s_FreeImageMutex;
    static int        s_FreeImageInitInstances = 0;

    static std::map<String, FREE_IMAGE_FORMAT> c_FreeImageExtensions{
        { "", FREE_IMAGE_FORMAT::FIF_RAW },
        { ".bmp", FREE_IMAGE_FORMAT::FIF_BMP },
        { ".ico", FREE_IMAGE_FORMAT::FIF_ICO },
        { ".jpeg", FREE_IMAGE_FORMAT::FIF_JPEG },
        { ".jng", FREE_IMAGE_FORMAT::FIF_JNG },
        { ".koa", FREE_IMAGE_FORMAT::FIF_KOALA },
        { ".lbm", FREE_IMAGE_FORMAT::FIF_LBM },
        { ".iff", FREE_IMAGE_FORMAT::FIF_IFF },
        { ".mng", FREE_IMAGE_FORMAT::FIF_MNG },
        { ".pbm", FREE_IMAGE_FORMAT::FIF_PBMRAW },
        { ".pcd", FREE_IMAGE_FORMAT::FIF_PCD },
        { ".pcx", FREE_IMAGE_FORMAT::FIF_PCX },
        { ".pgm", FREE_IMAGE_FORMAT::FIF_PGMRAW },
        { ".png", FREE_IMAGE_FORMAT::FIF_PNG },
        { ".ppm", FREE_IMAGE_FORMAT::FIF_PPMRAW },
        { ".ras", FREE_IMAGE_FORMAT::FIF_RAS },
        { ".targa", FREE_IMAGE_FORMAT::FIF_TARGA },
        { ".tif", FREE_IMAGE_FORMAT::FIF_TIFF },
        { ".tiff", FREE_IMAGE_FORMAT::FIF_TIFF },
        { ".wbmp", FREE_IMAGE_FORMAT::FIF_WBMP },
        { ".psd", FREE_IMAGE_FORMAT::FIF_PSD },
        { ".cut", FREE_IMAGE_FORMAT::FIF_CUT },
        { ".xbm", FREE_IMAGE_FORMAT::FIF_XBM },
        { ".xpm", FREE_IMAGE_FORMAT::FIF_XPM },
        { ".dds", FREE_IMAGE_FORMAT::FIF_DDS },
        { ".gif", FREE_IMAGE_FORMAT::FIF_GIF },
        { ".hdr", FREE_IMAGE_FORMAT::FIF_HDR },
        { ".sgi", FREE_IMAGE_FORMAT::FIF_SGI },
        { ".exr", FREE_IMAGE_FORMAT::FIF_EXR },
        { ".j2k", FREE_IMAGE_FORMAT::FIF_J2K },
        { ".j2c", FREE_IMAGE_FORMAT::FIF_J2K },
        { ".jp2", FREE_IMAGE_FORMAT::FIF_JP2 },
        { ".pfm", FREE_IMAGE_FORMAT::FIF_PFM },
        { ".pict", FREE_IMAGE_FORMAT::FIF_PICT },
        { ".webp", FREE_IMAGE_FORMAT::FIF_WEBP },
        { ".jxr", FREE_IMAGE_FORMAT::FIF_JXR }
    };

    //

    FreeImageInstance::FreeImageInstance()
    {
        std::scoped_lock initLock(s_FreeImageMutex);
        if (!s_FreeImageInitInstances++)
        {
            FreeImage_Initialise();
        }
    }

    FreeImageInstance::~FreeImageInstance()
    {
        std::scoped_lock initLock(s_FreeImageMutex);
        if (!--s_FreeImageInitInstances)
        {
            FreeImage_DeInitialise();
        }
    }

    //

    FREE_IMAGE_FORMAT FreeImageInstance::FormatFromExtension(
        const String& extension)
    {
        auto it = c_FreeImageExtensions.find(extension);
        return it != c_FreeImageExtensions.end() ? it->second : FREE_IMAGE_FORMAT::FIF_UNKNOWN;
    }
} // namespace Ame::Extensions