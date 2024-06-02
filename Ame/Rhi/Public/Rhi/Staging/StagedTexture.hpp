#pragma once

#include <Rhi/Staging/Desc.hpp>
#include <Rhi/Resource/Buffer.hpp>

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

    class StagedTexture
    {
    public:
        StagedTexture() = default;

        StagedTexture(
            Device&            rhiDevice,
            const TextureDesc& desc,
            StagedAccessType   accessType);

        StagedTexture(
            Device&            rhiDevice,
            const TextureDesc& desc,
            size_t             size,
            size_t             offset,
            Buffer             buffer);

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
        /// Get the underlying buffer.
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer() const;

        /// <summary>
        /// Get the underlying buffer.
        /// </summary>
        [[nodiscard]] Buffer& GetBuffer();

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
            size_t             textureSize,
            size_t             relativeOffset);

    private:
        const DeviceDesc* m_DeviceDesc = nullptr;
        TextureDesc       m_Desc;

        size_t m_TextureSize = 0;
        Buffer m_TextureBuffer;

        StagedTextureRegionList m_Regions;
    };
} // namespace Ame::Rhi::Staging