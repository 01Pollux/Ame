#include <Image/Image.hpp>
#include <Image/FreeImageUtils.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Dg::TextureData Image::GetTextureData(
        Dg::IDeviceContext* deviceContext) const noexcept
    {
        return Dg::TextureData();
    }

    //

    ImageFormat Image::GetFormat() const
    {
        return m_Format;
    }

    Math::Vector2U Image::GetSize() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return { FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap) };
    }

    uint32_t Image::GetLine() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_GetPitch(bitmap);
    }

    uint32_t Image::GetPitch() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_GetPitch(bitmap);
    }

    uint32_t Image::GetMemorySize() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_GetDIBSize(bitmap);
    }

    //

    Image Image::Clone() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        auto clone  = FreeImage_Clone(bitmap);
        return Image::Wrap(clone, m_Format, false, true);
    }

    Image Image::Clone(const Math::RectI& rect) const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        auto clone  = FreeImage_Copy(bitmap, rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
        return Image::Wrap(clone, m_Format, false, true);
    }

    bool Image::Paste(const Image& srcImage, const Math::RectI& rect)
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        Log::Rhi().Assert(!srcImage.IsArray(), "Source image is a bitmap array");

        auto dstBitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        auto srcBitmap = FreeImageUtils::GetBitmap(srcImage.m_BitmapData);

        return FreeImage_Paste(dstBitmap, srcBitmap, rect.Left(), rect.Top(), 0);
    }

    Image Image::Rescale(const Math::Vector2U size, ImageFilterType filter)
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap   = FreeImageUtils::GetBitmap(m_BitmapData);
        auto fiFilter = FreeImageUtils::ConvertFilter(filter);
        auto rescale  = FreeImage_Rescale(bitmap, size.x(), size.y(), fiFilter);

        return Image::Wrap(rescale, m_Format, false, true);
    }

    Image Image::View(const Math::RectI& rect) const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");
        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        auto view   = FreeImage_Copy(bitmap, rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
        return Image::Wrap(view, m_Format, false, true);
    }

    //

    Image Image::ConvertTo(ImageDataType dataType) const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        auto type   = FreeImageUtils::ConvertType(dataType);

        auto convert = FreeImage_ConvertToType(bitmap, type);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo4Bits() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo4Bits(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo8Bits() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo8Bits(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertToGreyscale() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertToGreyscale(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo16Bits555() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo16Bits555(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo16Bits565() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo16Bits565(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo24Bits() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo24Bits(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::ConvertTo32Bits() const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_ConvertTo32Bits(bitmap);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::Threshold(uint8_t value) const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_Threshold(bitmap, value);
        return Image::Wrap(convert, m_Format, false, true);
    }

    Image Image::Dither(ImageDitherType algorithm) const
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap  = FreeImageUtils::GetBitmap(m_BitmapData);
        auto convert = FreeImage_Dither(bitmap, static_cast<FREE_IMAGE_DITHER>(algorithm));
        return Image::Wrap(convert, m_Format, false, true);
    }

    bool Image::FlipHorizontal()
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_FlipHorizontal(bitmap);
    }

    bool Image::FlipVertical()
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_FlipVertical(bitmap);
    }

    bool Image::Invert()
    {
        Log::Rhi().Assert(!IsArray(), "Image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
        return FreeImage_Invert(bitmap);
    }

    bool Image::IsArray() const noexcept
    {
        return m_IsImageArray;
    }

    //

    void Image::Append(const Image& image)
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");
        Log::Rhi().Assert(!image.IsArray(), "Source image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        auto src    = FreeImageUtils::GetBitmap(image.m_BitmapData);

        FreeImage_AppendPage(bitmap, src);
    }

    void Image::Add(uint32_t page, const Image& image)
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");
        Log::Rhi().Assert(!image.IsArray(), "Source image is a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        auto src    = FreeImageUtils::GetBitmap(image.m_BitmapData);

        FreeImage_InsertPage(bitmap, page, src);
    }

    void Image::Remove(uint32_t page)
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        FreeImage_DeletePage(bitmap, page);
    }

    Image Image::Lock(int page) const
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        auto lock   = FreeImage_LockPage(bitmap, page);
        return Image::Wrap(lock, m_Format, false, true);
    }

    void Image::Unlock(const Image& image, bool changed) const
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");
        Log::Rhi().Assert(!image.IsArray(), "Source image is a bitmap array");

        auto bitmap      = FreeImageUtils::GetBitmapArray(m_BitmapData);
        auto imageBitmap = FreeImageUtils::GetBitmap(image.m_BitmapData);

        FreeImage_UnlockPage(bitmap, imageBitmap, changed);
    }

    bool Image::Move(int srcPage, int dstPage)
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        return FreeImage_MovePage(bitmap, srcPage, dstPage);
    }

    int Image::GetPagesCount() const
    {
        Log::Rhi().Assert(IsArray(), "Image is not a bitmap array");

        auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
        return FreeImage_GetPageCount(bitmap);
    }

    void* Image::GetBitmap() const
    {
        return m_BitmapData;
    }

    Image Image::Wrap(void* bitmap, ImageFormat format, bool isImageArray, bool ownsBitmapData)
    {
        return Image(bitmap, format, isImageArray, ownsBitmapData);
    }

    void Image::InitializeSubresource()
    {
        if (!IsArray())
        {
            // iterate through mips in freeimage
            auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);

            m_Subresource.pData       = FreeImage_GetBits(bitmap);
            m_Subresource.Stride      = FreeImage_GetPitch(bitmap);
            m_Subresource.DepthStride = 0; // TODO: support 1D/3D textures
        }
    }

    void Image::Release() noexcept
    {
        if (m_OwnsBitmapData)
        {
            try
            {
                if (IsArray())
                {
                    auto bitmap = FreeImageUtils::GetBitmapArray(m_BitmapData);
                    FreeImage_CloseMultiBitmap(bitmap);
                }
                else
                {
                    auto bitmap = FreeImageUtils::GetBitmap(m_BitmapData);
                    FreeImage_Unload(bitmap);
                }
            }
            catch (...)
            {
                Log::Rhi().Error("Failed to release image");
            }
        }
    }

    //

    ImageMipChain::ImageMipChain(Image image, uint32_t maxMipCount, ImageFilterType filter)
    {
        m_Images.emplace_back(std::move(image));

        auto size = m_Images.front().GetSize();
        while (size.x() > 1 && size.y() > 1)
        {
            if (maxMipCount && (m_Images.size() < maxMipCount))
            {
                break;
            }

            size.x() = std::max(1u, size.x() / 2);
            size.y() = std::max(1u, size.y() / 2);

            m_Images.emplace_back(m_Images.back().Rescale(size, filter));
        }
    }
} // namespace Ame::Rhi