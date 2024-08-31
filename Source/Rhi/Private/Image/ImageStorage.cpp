#include <Image/ImageStorage.hpp>
#include <Image/FreeImageUtils.hpp>

namespace Ame::Rhi
{
    ImageFormat ImageStorage::GetFormat(const char* format)
    {
        FreeImageUtils::Initialize();
        return FreeImageUtils::ConvertFormat(FreeImage_GetFIFFromFormat(format));
    }

    ImageFormat ImageStorage::GetFormatFromFileName(const char* fileName)
    {
        FreeImageUtils::Initialize();
        return FreeImageUtils::ConvertFormat(FreeImage_GetFIFFromFilename(fileName));
    }

    ImageFormat ImageStorage::GetFormat(std::istream& stream)
    {
        FreeImageUtils::Initialize();

        auto [io, handle] = FreeImageUtils::GetIO(&stream, nullptr);
        auto fif          = FreeImage_GetFileTypeFromHandle(&io, &handle, 0);

        return FreeImageUtils::ConvertFormat(fif);
    }

    //

    bool ImageStorage::Encode(const Image& image, std::ostream& stream, ImageEncodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat     = FreeImageUtils::ConvertFormat(image.GetFormat());
        auto fiFlags      = FreeImageUtils::ConvertFlags(flags);
        auto [io, handle] = FreeImageUtils::GetIO(nullptr, &stream);

        switch (image.GetType())
        {
        case ImageReferenceType::Local:
        {
            auto bitmap = FreeImageUtils::GetBitmap(image.GetBitmap());
            return FreeImage_SaveToHandle(fiFormat, bitmap, &io, &handle, fiFlags);
        }
        case ImageReferenceType::MultiBitmap:
        {
            auto bitmap = FreeImageUtils::GetBitmapArray(image.GetBitmap());
            return FreeImage_SaveMultiBitmapToHandle(fiFormat, bitmap, &io, &handle, fiFlags);
        }
        default:
        {
            return false;
        }
        }
    }

    ImageMemory ImageStorage::Load(ImageFormat format, std::byte* data, size_t size, ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(format);
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto mem    = FreeImage_OpenMemory(std::bit_cast<BYTE*>(data), static_cast<DWORD>(size));
        auto bitmap = FreeImage_LoadFromMemory(fiFormat, mem, fiFlags);

        return ImageMemory::Wrap(mem, bitmap, format);
    }

    ImageMemory ImageStorage::Load(std::byte* data, size_t size, ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto mem = FreeImage_OpenMemory(std::bit_cast<BYTE*>(data), static_cast<DWORD>(size));

        auto fif     = FreeImage_GetFileTypeFromMemory(mem, static_cast<DWORD>(size));
        auto format  = FreeImageUtils::ConvertFormat(fif);
        auto fiFlags = FreeImageUtils::ConvertFlags(flags);

        auto bitmap = FreeImage_LoadFromMemory(fif, mem, fiFlags);

        return ImageMemory::Wrap(mem, bitmap, format);
    }

    Image ImageStorage::Decode(ImageFormat format, std::istream& stream, ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(format);
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto [io, handle] = FreeImageUtils::GetIO(&stream, nullptr);
        auto bitmap       = FreeImage_LoadFromHandle(fiFormat, &io, &handle, fiFlags);

        return Image::Wrap(bitmap, format, ImageReferenceType::Local, true);
    }

    Image ImageStorage::DecodeArray(ImageFormat format, std::istream& stream, ImageDecodeFlags flags)
    {
        FreeImageUtils::Initialize();

        auto fiFormat = FreeImageUtils::ConvertFormat(format);
        auto fiFlags  = FreeImageUtils::ConvertFlags(flags);

        auto [io, handle] = FreeImageUtils::GetIO(&stream, nullptr);
        auto bitmap       = FreeImage_OpenMultiBitmapFromHandle(fiFormat, &io, &handle, fiFlags);

        return Image::Wrap(bitmap, format, ImageReferenceType::MultiBitmap, true);
    }
} // namespace Ame::Rhi