#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            snstanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
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
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount = 1) noexcept;
} // namespace Ame::Rhi