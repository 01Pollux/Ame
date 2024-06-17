#include <Gfx/Cache/MaterialBindingCache.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Util/ResourceSize.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

    MaterialBindingCache::MaterialBindingCache(
        EngineFrame&                     engineFrame,
        Rhi::Device&                     rhiDevice,
        const BufferAllocator::DescType& desc) :
        m_Device(rhiDevice),
        m_EndFrameHandle(engineFrame.OnUpdate({ &MaterialBindingCache::ResetFrameCache, this })),
        m_DynamicBuffer(rhiDevice, desc)
    {
    }

    void MaterialBindingCache::Bind(
        Rhi::CommandList&        commandList,
        const Shading::Material& material)
    {
        commandList.SetPipelineLayout(material.GetPipelineLayout());

        auto& setCache = m_SetCaches[material.GetPropertyHash()];
        if (!setCache.WasSet)
        {
            CreatePropertyBlock(commandList, material, setCache);
            setCache.WasSet = true;
        }

        if (setCache.DescriptorSet)
        {
            commandList.SetDescriptorSet(
                CD::c_MaterialData_SetIndex,
                setCache.DescriptorSet,
                setCache.DynamicBufferOffset == InvalidDynamicBufferOffset ? nullptr : &setCache.DynamicBufferOffset);
        }
    }

    //

    void MaterialBindingCache::ResetFrameCache()
    {
        m_DynamicBuffer.Reset();
        m_SetCaches.clear();
    }

    //

    void MaterialBindingCache::CreatePropertyBlock(
        Rhi::CommandList&        commandList,
        const Shading::Material& material,
        SetCache&                setCache)
    {
        uint32_t bufferSize = material.GetSizeOfUserData();

        std::vector<const nri::Descriptor*> descriptors;
        for (auto& iter : material.GetResources())
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
            uint32_t blockSize = Rhi::Util::GetConstantBufferSize(m_Device.get().GetDesc(), bufferSize);
            auto     handle    = m_DynamicBuffer.Rent(blockSize);

            auto descriptor = GetOrCreateConstantBufferDescriptor(handle);
            setCache.DescriptorSet.SetDynamicBuffer(0, descriptor);

            setCache.DynamicBufferOffset = handle.Offset;

            auto bufferPtr = std::bit_cast<std::byte*>(m_DynamicBuffer.GetBuffer(handle).GetPtr(handle.Offset));
            auto userData  = std::bit_cast<const std::byte*>(material.GetUserData());

            std::copy(userData, userData + bufferSize, bufferPtr);
        }

        if (!descriptors.empty())
        {
            std::vector<Rhi::DescriptorRangeUpdateDesc> rangeUpdateDescs;
            rangeUpdateDescs.reserve(descriptors.size());

            for (uint32_t i = 0; i < descriptors.size(); ++i)
            {
                auto& range         = rangeUpdateDescs.emplace_back();
                range.offsetInRange = 0;
                range.descriptors   = &descriptors[i];
                range.descriptorNum = 1;
            }
            setCache.DescriptorSet.SetRanges(0, rangeUpdateDescs);
        }
    }

    const nri::Descriptor* MaterialBindingCache::GetOrCreateConstantBufferDescriptor(
        const BufferAllocator::Handle& handle)
    {
        auto& view = m_DynamicBufferDescriptors[handle.BlockSlot];
        if (!view)
        {
            auto&            buffer = m_DynamicBuffer.GetBuffer(handle.BlockSlot);
            Rhi::BufferRange range(handle.Offset, handle.Size);
            view = buffer.CreateView({ .Range = range });
        }
        return view.Unwrap();
    }
} // namespace Ame::Gfx::Cache
