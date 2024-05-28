#include <Rhi/Util/ResourceSize.hpp>
#include <Rhi/Device/Device.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi::Util
{
    size_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.uploadBufferTextureRowAlignment);
    }

    size_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.uploadBufferTextureSliceAlignment);
    }

    size_t GetTypedBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.typedBufferOffsetAlignment);
    }

    size_t GetConstantBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.constantBufferOffsetAlignment);
    }

    size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.storageBufferOffsetAlignment);
    }
} // namespace Ame::Rhi::Util