#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureRowSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetTypedufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetConstantBufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount = 1) noexcept;

    /// <summary>
    /// Get the size of a constant buffer
    /// </summary>
    [[nodiscard]] size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount = 1) noexcept;
} // namespace Ame::Rhi