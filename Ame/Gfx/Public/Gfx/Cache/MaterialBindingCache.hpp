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
        using BlockBuffer              = Rhi::Util::BlockBasedBuffer;
        using BlockBufferDescriptorMap = std::map<uint32_t, Rhi::ResourceView>;

    public:
        MaterialBindingCache(
            Rhi::Device& Device,
            EngineFrame& Frame);

    public:
        /// <summary>
        /// Bind the material to the command list
        /// It either get the descriptor set from the cache or create a new one
        /// </summary>
        void Bind(
            Rhi::CommandList&        CommandList,
            const Shading::Material& Material);

    private:
        /// <summary>
        /// Create a new descriptor set for the material
        /// </summary>
        void CreatePropertyBlock(
            Rhi::CommandList&        CommandList,
            const Shading::Material& Material,
            SetCache&                Set);

        /// <summary>
        /// Get or create a descriptor set for the buffer
        /// </summary>
        [[nodiscard]] const nri::Descriptor* GetOrCreateConstantBufferDescriptor(
            uint32_t BlockSlot);

    private:
        Signals::OnEndFrame::Handle m_EndFrameHandle;

        SetCacheMap              m_SetCache;
        BlockBuffer              m_DynamicBuffer;
        BlockBufferDescriptorMap m_DynamicBufferDescriptors;
    };
} // namespace Ame::Gfx::Cache