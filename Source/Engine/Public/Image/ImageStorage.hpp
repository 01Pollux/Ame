#pragma once

#include <Image/Image.hpp>
#include <ostream>
#include <istream>

namespace Ame::Rhi
{
    struct AME_ENGINE_API ImageStorage
    {
        [[nodiscard]] static ImageFormat GetFormat(const char* format);
        [[nodiscard]] static ImageFormat GetFormatFromFileName(const char* fileName);
        [[nodiscard]] static ImageFormat GetFormat(std::istream& stream);

        static bool Encode(const Image& image, std::ostream& stream, ImageEncodeFlags flags = ImageEncodeFlags::None);

        [[nodiscard]] static ImageMemory Load(ImageFormat format, std::byte* data, size_t size,
                                              ImageDecodeFlags flags = ImageDecodeFlags::None);

        [[nodiscard]] static ImageMemory Load(std::byte* data, size_t size,
                                              ImageDecodeFlags flags = ImageDecodeFlags::None);

        [[nodiscard]] static Image Decode(ImageFormat format, std::istream& stream,
                                          ImageDecodeFlags flags = ImageDecodeFlags::None);

        [[nodiscard]] static Image DecodeArray(ImageFormat format, std::istream& stream,
                                               ImageDecodeFlags flags = ImageDecodeFlags::None);
    };
} // namespace Ame::Rhi