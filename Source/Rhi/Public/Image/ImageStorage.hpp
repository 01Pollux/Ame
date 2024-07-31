#pragma once

#include <Image/Image.hpp>
#include <ostream>
#include <istream>

namespace Ame::Rhi
{
    struct ImageStorage
    {
        [[nodiscard]] static ImageFormat GetFormat(
            const char* format);
        [[nodiscard]] static ImageFormat GetFormatFromFileName(
            const char* fileName);
        [[nodiscard]] static ImageFormat GetFormat(
            std::istream& stream);

        static bool Encode(
            const Image&     image,
            std::ostream&    stream,
            ImageEncodeFlags flags = ImageEncodeFlags::None);

        [[nodiscard]] static Image Decode(
            ImageFormat      format,
            std::istream&    stream,
            ImageDecodeFlags flags = ImageDecodeFlags::None);

        [[nodiscard]] static Image DecodeArray(
            ImageFormat      format,
            std::istream&    stream,
            ImageDecodeFlags flags = ImageDecodeFlags::None);
    };
} // namespace Ame::Rhi