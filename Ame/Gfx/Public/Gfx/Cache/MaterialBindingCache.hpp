#pragma once

#include <Frame/EngineFrame.hpp>

#include <Rhi/Resource/Set.hpp>
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
    class MaterialBindingCache
    {
        static constexpr uint32_t InvalidDynamicBufferOffset = -1;

        struct SetCache
        {
            Rhi::DescriptorSet DescriptorSet;
            bool               WasSet              = false;
            uint32_t           DynamicBufferOffset = InvalidDynamicBufferOffset;
        };

        using SetCacheMap              = std::map<uint64_t, SetCache>;
        using BlockBufferDescriptorMap = std::map<uint32_t, Rhi::ResourceView>;

    public:
        using BufferAllocator = Rhi::Util::BlockBasedBuffer<>;

    public:
        MaterialBindingCache(
            EngineFrame&                     engineFrame,
            Rhi::Device&                     rhiDevice,
            const BufferAllocator::DescType& desc = {
                .Size       = 0xFF * 0x1000,
                .UsageFlags = Rhi ::BufferUsageBits::CONSTANT_BUFFER });

    public:
        /// <summary>
        /// Bind the material to the command list
        /// It either get the descriptor set from the cache or create a new one
        /// </summary>
        void Bind(
            Rhi::CommandList&        commandList,
            const Shading::Material& material);

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

        SetCacheMap              m_SetCaches;
        BufferAllocator          m_DynamicBuffer;
        BlockBufferDescriptorMap m_DynamicBufferDescriptors;
    };
} // namespace Ame::Gfx::Cache