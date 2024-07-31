#pragma once

#include <Math/Rect.hpp>
#include <Math/Vector.hpp>

#include <Image/Core.hpp>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class Image
    {
        friend class ImageStorage;

    public:
        Image() = default;

        Image(std::nullptr_t) noexcept
        {
        }

        Image(const Image& image) noexcept :
            m_BitmapData(image.m_BitmapData),
            m_Format(image.m_Format),
            m_Subresource(image.m_Subresource),
            m_IsImageArray(image.m_IsImageArray),
            m_OwnsBitmapData(false)
        {
        }

        Image(Image&& image) noexcept :
            m_BitmapData(image.m_BitmapData),
            m_Format(image.m_Format),
            m_Subresource(std::move(image.m_Subresource)),
            m_IsImageArray(image.m_IsImageArray),
            m_OwnsBitmapData(image.m_OwnsBitmapData)
        {
            image.m_BitmapData     = nullptr;
            image.m_OwnsBitmapData = false;
        }

        Image& operator=(const Image& image) noexcept
        {
            if (this != &image)
            {
                Release();

                m_BitmapData     = image.m_BitmapData;
                m_Format         = image.m_Format;
                m_Subresource    = image.m_Subresource;
                m_IsImageArray   = image.m_IsImageArray;
                m_OwnsBitmapData = false;
            }

            return *this;
        }

        Image& operator=(Image&& image) noexcept
        {
            if (this != &image)
            {
                Release();

                m_BitmapData     = image.m_BitmapData;
                m_Format         = image.m_Format;
                m_Subresource    = std::move(image.m_Subresource);
                m_IsImageArray   = image.m_IsImageArray;
                m_OwnsBitmapData = image.m_OwnsBitmapData;

                image.m_BitmapData     = nullptr;
                image.m_OwnsBitmapData = false;
            }

            return *this;
        }

        ~Image() noexcept
        {
            Release();
        }

    public:
        [[nodiscard]] Dg::TextureData GetTextureData(
            Dg::IDeviceContext* deviceContext = nullptr) const noexcept;

    public:
        [[nodiscard]] ImageFormat    GetFormat() const;
        [[nodiscard]] Math::Vector2U GetSize() const;
        [[nodiscard]] uint32_t       GetLine() const;
        [[nodiscard]] uint32_t       GetPitch() const;
        [[nodiscard]] uint32_t       GetMemorySize() const;

    public:
        [[nodiscard]] Image Clone() const;
        [[nodiscard]] Image Clone(const Math::RectI& rect) const;
        bool                Paste(const Image& srcImage, const Math::RectI& rect);
        [[nodiscard]] Image Rescale(const Math::Vector2U size, ImageFilterType filter = ImageFilterType::Catmullrom);
        [[nodiscard]] Image View(const Math::RectI& rect) const;

    public:
        [[nodiscard]] Image ConvertTo(ImageDataType dataType) const;
        [[nodiscard]] Image ConvertTo4Bits() const;
        [[nodiscard]] Image ConvertTo8Bits() const;
        [[nodiscard]] Image ConvertToGreyscale() const;
        [[nodiscard]] Image ConvertTo16Bits555() const;
        [[nodiscard]] Image ConvertTo16Bits565() const;
        [[nodiscard]] Image ConvertTo24Bits() const;
        [[nodiscard]] Image ConvertTo32Bits() const;
        [[nodiscard]] Image Threshold(uint8_t value) const;
        [[nodiscard]] Image Dither(ImageDitherType algorithm) const;

    public:
        bool FlipHorizontal();
        bool FlipVertical();
        bool Invert();

    public:
        [[nodiscard]] bool IsArray() const noexcept;

        void Append(const Image& image);
        void Add(uint32_t page, const Image& image);
        void Remove(uint32_t page);

        [[nodiscard]] Image Lock(int page) const;
        void                Unlock(const Image& image, bool changed = false) const;

        bool              Move(int srcPage, int dstPage);
        [[nodiscard]] int GetPagesCount() const;

    private:
        Image(void* bitmap, ImageFormat format, bool isImageArray, bool ownsBitmapData) :
            m_BitmapData(bitmap),
            m_Format(format),
            m_IsImageArray(isImageArray),
            m_OwnsBitmapData(ownsBitmapData)
        {
            InitializeSubresource();
        }

    private:
        [[nodiscard]] void*        GetBitmap() const;
        [[nodiscard]] static Image Wrap(void* bitmap, ImageFormat format, bool isImageArray, bool ownsBitmapData);

        void InitializeSubresource();
        void Release() noexcept;

    private:
        void*                 m_BitmapData = nullptr;
        ImageFormat           m_Format     = ImageFormat::Unknown;
        Dg::TextureSubResData m_Subresource;
        bool                  m_IsImageArray   : 1 = false;
        bool                  m_OwnsBitmapData : 1 = false;
    };

    class ImageMipChain
    {
    public:
        ImageMipChain() = default;

        ImageMipChain(Image image, uint32_t maxMipCount = 0, ImageFilterType filter = ImageFilterType::Catmullrom);

    private:
        std::vector<Image> m_Images;
    };
} // namespace Ame::Rhi