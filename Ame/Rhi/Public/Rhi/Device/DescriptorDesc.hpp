#pragma once

#include <cstdint>

namespace Ame::Rhi
{
    struct DescriptorAllocationDesc
    {
        uint32_t DescriptorSetMaxCount           = 16'384;
        uint32_t SamplerMaxCount                 = 256;
        uint32_t ConstantBufferMaxCount          = 16'384;
        uint32_t DynamicConstantBufferMaxCount   = 256;
        uint32_t TextureMaxCount                 = 16'384;
        uint32_t StorageTextureMaxCount          = 256;
        uint32_t BufferMaxCount                  = 16'384;
        uint32_t StorageBufferMaxCount           = 256;
        uint32_t StructuredBufferMaxCount        = 16'384;
        uint32_t StorageStructuredBufferMaxCount = 256;
        uint32_t AccelerationStructureMaxCount   = 256;
    };
} // namespace Ame::Rhi