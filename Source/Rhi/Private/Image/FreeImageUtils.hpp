#pragma once

#include <FreeImage.h>
#include <Image/Core.hpp>

namespace Ame::Rhi
{
    struct FreeImageUtils
    {
        struct IOStream
        {
            std::streampos Offset = 0;
            std::istream*  istream;
            std::ostream*  ostream;
        };

        static void Initialize();

        [[nodiscard]] static std::pair<FreeImageIO, IOStream> GetIO(std::istream* istream, std::ostream* ostream);

        [[nodiscard]] static FREE_IMAGE_FORMAT ConvertFormat(ImageFormat format);
        [[nodiscard]] static ImageFormat       ConvertFormat(FREE_IMAGE_FORMAT format);

        [[nodiscard]] static FREE_IMAGE_TYPE ConvertType(ImageDataType type);
        [[nodiscard]] static ImageDataType   ConvertType(FREE_IMAGE_TYPE type);

        [[nodiscard]] static FREE_IMAGE_FILTER ConvertFilter(ImageFilterType filter);

        [[nodiscard]] static int ConvertFlags(ImageEncodeFlags flags);
        [[nodiscard]] static int ConvertFlags(ImageDecodeFlags flags);

        [[nodiscard]] static FIBITMAP*      GetBitmap(void* bitmap);
        [[nodiscard]] static FIMEMORY*      GetMemory(void* bitmap);
        [[nodiscard]] static FIMULTIBITMAP* GetBitmapArray(void* bitmap);
    };
} // namespace Ame::Rhi