#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi::Util
{
    /// <summary>
    /// Get the size of a texture (width * height * depth)
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSize(
        const DeviceDesc&  desc,
        const TextureDesc& textureDesc,
        size_t             instanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a texture (width * height * depth)
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth) noexcept;

    /// <summary>
    /// Get the size of a texture (width * height * depth)
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth,
        uint32_t          mipIndex) noexcept;

    /// <summary>
    /// Get the size of a texture row (width)
    /// </summary>
    [[nodiscard]] uint32_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        uint32_t          size,
        size_t            instanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a texture slice (row * height)
    /// </summary>
    [[nodiscard]] uint32_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        uint32_t          size,
        size_t            instanceCount = 1) noexcept;

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