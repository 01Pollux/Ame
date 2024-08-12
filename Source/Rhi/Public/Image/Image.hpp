#pragma once

#include <Math/Rect.hpp>
#include <Math/Vector.hpp>

#include <Image/Core.hpp>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class Image
    {
    public:
        Image() = default;

        Image(std::nullptr_t) noexcept
        {
        }

        Image(const Image& image) noexcept :
            m_BitmapData(image.m_BitmapData),
            m_Format(image.m_Format),
            m_ReferenceType(image.m_ReferenceType),
            m_OwnsBitmapData(false)
        {
        }

        Image(Image&& image) noexcept :
            m_BitmapData(std::exchange(image.m_BitmapData, nullptr)),
            m_Format(image.m_Format),
            m_ReferenceType(image.m_ReferenceType),
            m_OwnsBitmapData(image.m_OwnsBitmapData)
        {
            image.m_OwnsBitmapData = false;
        }

        Image& operator=(const Image& image) noexcept
        {
            if (this != &image)
            {
                Release();

                m_BitmapData     = image.m_BitmapData;
                m_Format         = image.m_Format;
                m_ReferenceType  = image.m_ReferenceType;
                m_OwnsBitmapData = false;
            }

            return *this;
        }

        Image& operator=(Image&& image) noexcept
        {
            if (this != &image)
            {
                Release();

                m_BitmapData     = std::exchange(image.m_BitmapData, nullptr);
                m_Format         = image.m_Format;
                m_ReferenceType  = image.m_ReferenceType;
                m_OwnsBitmapData = image.m_OwnsBitmapData;

                image.m_OwnsBitmapData = false;
            }

            return *this;
        }

        ~Image() noexcept
        {
            Release();
        }

        explicit operator bool() const noexcept
        {
            return m_BitmapData != nullptr;
        }

    public:
        [[nodiscard]] ImageReferenceType GetType() const noexcept;

        [[nodiscard]] Dg::TextureSubResData GetSubresource() const noexcept;

    public:
        [[nodiscard]] ImageFormat      GetFormat() const;
        [[nodiscard]] Math::Vector2U   GetSize() const;
        [[nodiscard]] uint32_t         GetLine() const;
        [[nodiscard]] uint32_t         GetPitch() const;
        [[nodiscard]] std::byte*       GetPixels();
        [[nodiscard]] const std::byte* GetPixels() const;
        [[nodiscard]] uint32_t         GetMemorySize() const;

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
        void Append(const Image& image);
        void Add(uint32_t page, const Image& image);
        void Remove(uint32_t page);

        [[nodiscard]] Image Lock(int page) const;
        void                Unlock(const Image& image, bool changed = false) const;

        bool              Move(int srcPage, int dstPage);
        [[nodiscard]] int GetPagesCount() const;

    private:
        Image(void* bitmap, ImageFormat format, ImageReferenceType referenceType, bool ownsBitmapData) :
            m_BitmapData(bitmap),
            m_Format(format),
            m_ReferenceType(referenceType),
            m_OwnsBitmapData(ownsBitmapData)
        {
        }

    public:
        [[nodiscard]] void*        GetBitmap() const;
        [[nodiscard]] static Image Wrap(void* bitmap, ImageFormat format, ImageReferenceType referenceType, bool ownsBitmapData)
        {
            return Image(bitmap, format, referenceType, ownsBitmapData);
        }

    private:
        void Release() noexcept;

    private:
        void*              m_BitmapData         = nullptr;
        ImageFormat        m_Format             = ImageFormat::Unknown;
        ImageReferenceType m_ReferenceType      = ImageReferenceType::Unknown;
        bool               m_OwnsBitmapData : 1 = false;
    };

    class ImageMemory
    {
    public:
        ImageMemory() = default;

        [[nodiscard]] static ImageMemory Wrap(void* memory, void* bitmap, ImageFormat format)
        {
            return ImageMemory(memory, bitmap, format);
        }

        [[nodiscard]] const Image& GetMemory() const noexcept;
        [[nodiscard]] const Image& GetImage() const noexcept;
        [[nodiscard]] Image&       GetImage() noexcept;

    private:
        ImageMemory(void* memory, void* bitmap, ImageFormat format);

    private:
        Image m_Memory;
        Image m_Image;
    };

    class ImageMipChain
    {
    public:
        ImageMipChain() = default;

        ImageMipChain(Image image, uint32_t maxMipCount = 0, ImageFilterType filter = ImageFilterType::Catmullrom);

    public:
        [[nodiscard]] const Image& GetMip(uint32_t level) const noexcept;

        [[nodiscard]] uint32_t GetMipCount() const noexcept;

        [[nodiscard]] uint32_t GetMemorySize() const;

        [[nodiscard]] const std::span<const Image>       GetMips() const noexcept;
        [[nodiscard]] std::vector<Dg::TextureSubResData> GetSubresource() const;

    private:
        std::vector<Image> m_Images;
    };
} // namespace Ame::Rhi