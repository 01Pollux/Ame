#include <Rhi/Util/ResourceSize.hpp>
#include <Rhi/Device/Device.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi
{
    size_t GetUploadBufferTextureRowSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount) noexcept
    {
        return InstanceCount * Math::AlignUp(StructSize, Desc.uploadBufferTextureRowAlignment);
    }

    size_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount) noexcept
    {
        return InstanceCount * Math::AlignUp(StructSize, Desc.uploadBufferTextureSliceAlignment);
    }

    size_t GetTypedufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount) noexcept
    {
        return InstanceCount * Math::AlignUp(StructSize, Desc.typedBufferOffsetAlignment);
    }

    size_t GetConstantBufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount) noexcept
    {
        return InstanceCount * Math::AlignUp(StructSize, Desc.constantBufferOffsetAlignment);
    }

    size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& Desc,
        size_t            StructSize,
        size_t            InstanceCount) noexcept
    {
        return InstanceCount * Math::AlignUp(StructSize, Desc.storageBufferOffsetAlignment);
    }
} // namespace Ame::Rhi