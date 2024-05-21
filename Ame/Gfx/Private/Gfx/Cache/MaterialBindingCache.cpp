#include <Gfx/Cache/MaterialBindingCache.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>
#include <Rhi/CommandList/CommandList.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

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
        const Shading::Material& Material)
    {
        auto& Set = m_SetCache[Material.GetHash()];
        if (!Set.WasSet)
        {
            CreatePropertyBlock(CommandList, Material, Set);
            Set.WasSet = true;
        }

        CommandList.SetPipelineLayout(Material.GetPipelineLayout());

        if (Set.DescriptorSet)
        {
            CommandList.SetDescriptorSet(CD::MaterialData_SetIndex, Set.DescriptorSet, Set.DynamicBufferOffset == InvalidDynamicBufferOffset ? nullptr : &Set.DynamicBufferOffset);
        }
    }

    //

    void MaterialBindingCache::CreatePropertyBlock(
        Rhi::CommandList&        CommandList,
        const Shading::Material& Material,
        SetCache&                Set)
    {
        uint32_t BufferSize = Material.GetSizeOfUserData();

        std::vector<const nri::Descriptor*> Descriptors;
        for (auto Resource : Material.GetResources())
        {
            std::visit(
                VariantVisitor{
                    [&Descriptors](const auto& Buffer)
                    {
                        Descriptors.emplace_back(Buffer.View->Unwrap());
                    } },
                Resource->second);
        }

        bool HasMaterialData = BufferSize || !Descriptors.empty();
        if (HasMaterialData)
        {
            Set.DescriptorSet = CommandList.AllocateSet(CD::MaterialData_SetIndex);
        }

        if (BufferSize)
        {
            auto Handle = m_DynamicBuffer.Rent(BufferSize);

            auto Descriptor = GetOrCreateConstantBufferDescriptor(Handle.BlockSlot);
            Set.DescriptorSet.SetDynamicBuffer(0, Descriptor);

            Set.DynamicBufferOffset = Handle.Offset;

            auto BufferPtr = std::bit_cast<uint8_t*>(m_DynamicBuffer.GetBuffer(Handle).GetPtr());
            auto UserData  = std::bit_cast<const uint8_t*>(Material.GetUserData());

            std::copy(UserData, UserData + BufferSize, BufferPtr);
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
