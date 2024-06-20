#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Staging/Desc.hpp>

namespace Ame::Rhi::Staging
{
    struct StagedTextureRegion
    {
        CRef<FormatProps> Props;
        size_t            Offset;
        uint32_t          RowPitch;
        Rhi::Dim_t        Width;
        Rhi::Dim_t        Height;
        Rhi::Dim_t        Depth;

        [[nodiscard]] size_t SlicePitch() const noexcept
        {
            return RowPitch * Height * Props.get().blockHeight;
        }

        [[nodiscard]] size_t Size() const noexcept
        {
            return SlicePitch() * Depth;
        }

        [[nodiscard]] size_t OffsetAt(
            uint32_t x,
            uint32_t y = 0,
            uint32_t z = 0) const noexcept
        {
            return Offset +
                   SlicePitch() * z +
                   RowPitch * y * Props.get().blockHeight +
                   x * Props.get().blockWidth * Props.get().stride;
        }
    };

    class StagedTexture : public Buffer
    {
    public:
        [[nodiscard]] static StagedTexture Create(
            DeviceResourceAllocator& allocator,
            const TextureDesc&       desc,
            StagedAccessType         accessType);

        /// <summary>
        /// Create a staged texture for the specified external buffer, the staged texture must not be released.
        /// </summary>
        [[nodiscard]] static StagedTexture CreateForView(
            const Rhi::DeviceDesc& deviceDesc,
            const TextureDesc&     desc,
            Buffer                 buffer,
            size_t                 totalSize,
            size_t                 offset);

    public:
        StagedTexture() = default;
        StagedTexture(std::nullptr_t)
        {
        }

    protected:
        StagedTexture(
            const Rhi::DeviceDesc& deviceDesc,
            const TextureDesc&     desc,
            Buffer                 buffer,
            size_t                 totalSize,
            size_t                 offset);

    public:
        /// <summary>
        /// Get the texture size for the specified desc.
        /// </summary>
        static size_t GetTextureSize(
            const DeviceDesc&  deviceDesc,
            const TextureDesc& textureDesc);

    public:
        /// <summary>
        /// Get the texture desc.
        /// </summary>
        [[nodiscard]] const TextureDesc& GetTextureDesc() const;

        /// <summary>
        /// Get the buffer size.
        /// </summary>
        [[nodiscard]] size_t GetBufferSize() const;

        /// <summary>
        /// Get the region at the specified mip level and array index.
        /// </summary>
        [[nodiscard]] const StagedTextureRegion& GetRegion(
            uint32_t mipLevel   = 0,
            uint32_t arrayIndex = 0) const;

        /// <summary>
        /// Get the region at the specified mip level and array index and rect.
        /// </summary>
        [[nodiscard]] StagedTextureRegion GetRegion(
            const TextureRect& rect,
            uint32_t           mipLevel   = 0,
            uint32_t           arrayIndex = 0) const;

    private:
        using StagedTextureRegionList = std::vector<StagedTextureRegion>;

    private:
        [[nodiscard]] static StagedTextureRegionList CreateRegions(
            const DeviceDesc&  deviceDesc,
            const TextureDesc& textureDesc,
            size_t             totalSize,
            size_t             relativeOffset);

    private:
        const Rhi::DeviceDesc*  m_DeviceDesc = nullptr;
        TextureDesc             m_Desc{};
        size_t                  m_TextureSize = 0;
        StagedTextureRegionList m_Regions;
    };

    AME_RHI_SCOPED_RESOURCE(StagedTexture);
} // namespace Ame::Rhi::Staging
