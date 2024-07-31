#include <Image/ImageStorage.hpp>
#include <Image/FreeImageUtils.hpp>

namespace Ame::Rhi
{
    ImageFormat ImageStorage::GetFormat(
        const char* format)
    {
        FreeImageUtils::Initialize();
        return FreeImageUtils::ConvertFormat(FreeImage_GetFIFFromFormat(format));
    }

    ImageFormat ImageStorage::GetFormatFromFileName(
        const char* fileName)
    {
        FreeImageUtils::Initialize();
        return FreeImageUtils::ConvertFormat(FreeImage_GetFIFFromFilename(fileName));
    }

    ImageFormat ImageStorage::GetFormat(
        std::istream& stream)
    {
        FreeImageUtils::Initialize();

        auto io  = FreeImageUtils::GetIO();
        auto fif = FreeImage_GetFileTypeFromHandle(&io, &stream, 0);

        return FreeImageUtils::ConvertFormat(fif);
    }

    //

    bool ImageStorage::Encode(
        const Image&     image,
        std::ostream&    stream,
        ImageEncodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(image.GetFormat());
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto io = FreeImageUtils::GetIO();
        if (image.IsArray())
        {
            auto bitmap = FreeImageUtils::GetBitmap(image.GetBitmap());
            return FreeImage_SaveToHandle(fiFormat, bitmap, &io, &stream, fiFlags);
        }
        else
        {
            auto bitmap = FreeImageUtils::GetBitmapArray(image.GetBitmap());
            return FreeImage_SaveMultiBitmapToHandle(fiFormat, bitmap, &io, &stream, fiFlags);
        }
    }

    Image ImageStorage::Decode(
        ImageFormat      format,
        std::istream&    stream,
        ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(format);
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto io     = FreeImageUtils::GetIO();
        auto bitmap = FreeImage_LoadFromHandle(fiFormat, &io, &stream, fiFlags);

        return Image::Wrap(bitmap, format, false, true);
    }

    Image ImageStorage::DecodeArray(
        ImageFormat      format,
        std::istream&    stream,
        ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(format);
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto io     = FreeImageUtils::GetIO();
        auto bitmap = FreeImage_OpenMultiBitmapFromHandle(fiFormat, &io, &stream, fiFlags);

        return Image::Wrap(bitmap, format, true, true);
    }
} // namespace Ame::Rhi