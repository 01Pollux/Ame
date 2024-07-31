#pragma once

#include <FreeImage.h>
#include <Image/Core.hpp>

namespace Ame::Rhi
{
    struct FreeImageUtils
    {
        static void Initialize();

        [[nodiscard]] static FreeImageIO GetIO() noexcept;

        [[nodiscard]] static FREE_IMAGE_FORMAT ConvertFormat(ImageFormat format);
        [[nodiscard]] static ImageFormat       ConvertFormat(FREE_IMAGE_FORMAT format);

        [[nodiscard]] static FREE_IMAGE_TYPE ConvertType(ImageDataType type);

        [[nodiscard]] static FREE_IMAGE_FILTER ConvertFilter(ImageFilterType filter);

        [[nodiscard]] static int ConvertFlags(ImageEncodeFlags flags);
        [[nodiscard]] static int ConvertFlags(ImageDecodeFlags flags);

        [[nodiscard]] static FIBITMAP*      GetBitmap(void* bitmap);
        [[nodiscard]] static FIMULTIBITMAP* GetBitmapArray(void* bitmap);
    };
} // namespace Ame::Rhi