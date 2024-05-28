#include <Gfx/Cache/MaterialBindingCache.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>
#include <Rhi/CommandList/CommandList.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

    MaterialBindingCache::MaterialBindingCache(
        Rhi::Device& rhiDevice,
        EngineFrame& engineFrame) :
        m_DynamicBuffer(rhiDevice)
    {
        m_EndFrameHandle = {
            engineFrame.OnUpdate()
                .ObjectSignal(),
            [this]
            {
                m_SetCaches.clear();
            }
        };
    }

    void MaterialBindingCache::Bind(
        Rhi::CommandList&        commandList,
        const Shading::Material& material)
    {
        auto& setCache = m_SetCaches[material.GetPropertyHash()];
        if (!setCache.WasSet)
        {
            CreatePropertyBlock(commandList, material, setCache);
            setCache.WasSet = true;
        }

        commandList.SetPipelineLayout(material.GetPipelineLayout());

        if (setCache.DescriptorSet)
        {
            commandList.SetDescriptorSet(
                CD::c_MaterialData_SetIndex,
                setCache.DescriptorSet,
                setCache.DynamicBufferOffset == InvalidDynamicBufferOffset ? nullptr : &setCache.DynamicBufferOffset);
        }
    }

    //

    void MaterialBindingCache::CreatePropertyBlock(
        Rhi::CommandList&        commandList,
        const Shading::Material& material,
        SetCache&                setCache)
    {
        uint32_t bufferSize = material.GetSizeOfUserData();

        std::vector<const nri::Descriptor*> descriptors;
        for (auto iter : material.GetResources())
        {
            std::visit(
                VariantVisitor{
                    [&descriptors](const auto& resource)
                    {
                        descriptors.emplace_back(resource.View->Unwrap());
                    } },
                iter->second);
        }

        bool hasMaterialData = bufferSize || !descriptors.empty();
        if (hasMaterialData)
        {
            setCache.DescriptorSet = commandList.AllocateSet(CD::c_MaterialData_SetIndex);
        }

        if (bufferSize)
        {
            auto handle = m_DynamicBuffer.Rent(bufferSize);

            auto descriptor = GetOrCreateConstantBufferDescriptor(handle.BlockSlot);
            setCache.DescriptorSet.SetDynamicBuffer(0, descriptor);

            setCache.DynamicBufferOffset = handle.Offset;

            auto bufferPtr = std::bit_cast<uint8_t*>(m_DynamicBuffer.GetBuffer(handle).GetPtr());
            auto userData  = std::bit_cast<const uint8_t*>(material.GetUserData());

            std::copy(userData, userData + bufferSize, bufferPtr);
        }

        if (!descriptors.empty())
        {
            setCache.DescriptorSet.SetRange(0, { descriptors.data(), Rhi::Count32(descriptors) });
        }
    }

    const nri::Descriptor* MaterialBindingCache::GetOrCreateConstantBufferDescriptor(
        uint32_t blockSlot)
    {
        auto& view = m_DynamicBufferDescriptors[blockSlot];
        if (!view)
        {
            view = m_DynamicBuffer.GetBuffer(blockSlot).CreateView({});
        }
        return view.Unwrap();
    }
} // namespace Ame::Gfx::Cache
