#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi::Util
{
    /// <summary>
    /// Get the size of a texture (width * height * depth) with mipmaps and array
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth,
        uint32_t          mipCount,
        uint32_t          arraySize) noexcept;

    /// <summary>
    /// Get texture's dimension at a specific mip level, returns 1 if value is 0
    /// </summary>
    [[nodiscard]] Dim_t GetTextureDimension(
        Dim_t dimension,
        Mip_t mipIndex) noexcept;

    //

    /// <summary>
    /// Get the byte size for upload buffer texture
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureFlatSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth) noexcept;

    /// <summary>
    /// Get the size of a texture (width * height * depth)
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth,
        uint32_t          mipIndex,
        uint32_t          arrayIndex) noexcept;

    //

    /// <summary>
    /// Get the size of a texture row (width)
    /// </summary>
    [[nodiscard]] uint32_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width) noexcept;

    /// <summary>
    /// Get the size of a texture slice (row * height)
    /// </summary>
    [[nodiscard]] uint32_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          rowSize,
        uint32_t          heigth) noexcept;

    //

    /// <summary>
    /// Get the size of a typed buffer
    /// </summary>
    [[nodiscard]] size_t GetTypedBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetConstantBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of an unordered access buffer
    /// </summary>
    [[nodiscard]] size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount = 1) noexcept;
} // namespace Ame::Rhi::Util