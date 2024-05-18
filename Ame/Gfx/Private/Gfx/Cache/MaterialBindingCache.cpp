#include <Gfx/Cache/MaterialBindingCache.hpp>

#include <Gfx/Shading/Material.hpp>
#include <Rhi/CommandList/CommandList.hpp>

namespace Ame::Gfx::Cache
{
    MaterialBindingCache::MaterialBindingCache(
        Rhi::Device& Device,
        EngineFrame& Frame) :
        m_DynamicBuffer(Device)
    {
        m_EndFrameHandle = {
            Frame.OnUpdate()
                .ObjectSignal(),
            [this]
            {
                m_SetCache.clear();
            }
        };
    }

    void MaterialBindingCache::Bind(
        Rhi::CommandList&        CommandList,
        const Shading::Material& Material,
        uint32_t                 MaterialSetIndex)
    {
        auto& Set = m_SetCache[Material.GetHash()];
        if (!Set.DescriptorSet)
        {
            CreatePropertyBlock(CommandList, Material, MaterialSetIndex, Set);
        }

        CommandList.SetDescriptorSet(MaterialSetIndex, Set.DescriptorSet, Set.DynamicBufferOffset == InvalidDynamicBufferOffset ? nullptr : &Set.DynamicBufferOffset);
    }

    //

    void MaterialBindingCache::CreatePropertyBlock(
        Rhi::CommandList&        CommandList,
        const Shading::Material& Material,
        uint32_t                 MaterialSetIndex,
        SetCache&                Set)
    {
        Set.DescriptorSet = CommandList.AllocateSet(MaterialSetIndex);

        uint32_t BufferSize = Material.GetSizeOfUserData();
        if (BufferSize)
        {
            auto Handle = m_DynamicBuffer.Rent(BufferSize);

            auto Descriptor = GetOrCreateConstantBufferDescriptor(Handle.BlockSlot);
            Set.DescriptorSet.SetDynamicBuffer(0, Descriptor);

            Set.DynamicBufferOffset = Handle.Offset;
        }

        std::vector<const nri::Descriptor*> Descriptors;
        for (auto Resource : Material.GetResources())
        {
            Descriptors.emplace_back(Resource.View.get().Unwrap());
        }
        if (!Descriptors.empty())
        {
            Set.DescriptorSet.SetRange(0, { Descriptors.data(), Rhi::Count32(Descriptors) });
        }
    }

    const nri::Descriptor* MaterialBindingCache::GetOrCreateConstantBufferDescriptor(
        uint32_t BlockSlot)
    {
        auto& View = m_DynamicBufferDescriptors[BlockSlot];
        if (!View)
        {
            View = m_DynamicBuffer.GetBuffer(BlockSlot).CreateView({});
        }
        return View.Unwrap();
    }
} // namespace Ame::Gfx::Cache
