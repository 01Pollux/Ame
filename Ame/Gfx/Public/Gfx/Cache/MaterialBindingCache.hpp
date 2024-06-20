#pragma once

#include <Frame/EngineFrame.hpp>

#include <Rhi/Resource/DescriptorSet.hpp>
#include <Rhi/Resource/DescriptorTable.hpp>
#include <Rhi/Util/BlockBasedBuffer.hpp>

namespace Ame::Rhi
{
    class CommandList;
} // namespace Ame::Rhi

namespace Ame::Gfx::Shading
{
    class Material;
} // namespace Ame::Gfx::Shading

namespace Ame::Gfx::Cache
{
    struct MaterialBindingCacheDesc
    {
    public:
        using BufferAllocator = Rhi::Util::BlockBasedBuffer<>;

        Rhi::DescriptorPoolDesc DescriptorPoolDesc{
            .descriptorSetMaxNum         = 16'384,
            .samplerMaxNum               = 256,
            .dynamicConstantBufferMaxNum = 65'536,
            .textureMaxNum               = 131'072,
            .structuredBufferMaxNum      = 65'536
        };
        BufferAllocator::DescType BufferAllocatorDesc{
            .Size       = 0xFF * 0x1000,
            .UsageFlags = Rhi ::BufferUsageBits::CONSTANT_BUFFER
        };
    };

    class MaterialBindingCache
    {
        static constexpr uint32_t InvalidDynamicBufferOffset = -1;

        struct SetCache
        {
            Rhi::DescriptorSet DescriptorSet;
            uint32_t           DynamicBufferOffset = InvalidDynamicBufferOffset;
            bool               WasSet              = false;
        };

        using SetCacheMap              = std::map<uint64_t, SetCache>;
        using BlockBufferDescriptorMap = std::map<uint32_t, Rhi::ScopedResourceView>;

        using BufferAllocator = MaterialBindingCacheDesc::BufferAllocator;

    public:
        MaterialBindingCache(
            EngineFrame&                    engineFrame,
            Rhi::Device&                    rhiDevice,
            const MaterialBindingCacheDesc& desc = {});

    public:
        /// <summary>
        /// Bind the material to the command list
        /// It either get the descriptor set from the cache or create a new one
        /// </summary>
        void Bind(
            Rhi::CommandList&        commandList,
            const Shading::Material& material);

        /// <summary>
        /// Allocate a new descriptor set
        /// </summary>
        [[nodiscard]] Rhi::DescriptorSet AllocateSet(
            const Shading::Material& material,
            uint32_t                 setIndex,
            uint32_t                 variableCount = 0);

    private:
        /// <summary>
        /// Reset buffer and set caches
        /// </summary>
        void ResetFrameCache();

    private:
        /// <summary>
        /// Create a new descriptor set for the material
        /// </summary>
        void CreatePropertyBlock(
            Rhi::CommandList&        commandList,
            const Shading::Material& material,
            SetCache&                setCache);

        /// <summary>
        /// Get or create a descriptor set for the buffer
        /// </summary>
        [[nodiscard]] const nri::Descriptor* GetOrCreateConstantBufferDescriptor(
            const BufferAllocator::Handle& handle);

    private:
        Ref<Rhi::Device> m_Device;

        Signals::ScopedConnection m_EndFrameHandle;

        Rhi::ScopedDescriptorTable m_DescriptorTable;

        SetCacheMap              m_SetCaches;
        BufferAllocator          m_DynamicBuffer;
        BlockBufferDescriptorMap m_DynamicBufferDescriptors;
    };
} // namespace Ame::Gfx::Cache