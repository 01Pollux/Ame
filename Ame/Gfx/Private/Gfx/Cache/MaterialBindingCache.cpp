#include <Gfx/Cache/MaterialBindingCache.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/Util/ResourceSize.hpp>

namespace Ame::Gfx::Cache
{
    namespace CD = Constants::DescriptorRanges;

    //

    MaterialBindingCache::MaterialBindingCache(
        EngineFrame&                    engineFrame,
        Rhi::Device&                    rhiDevice,
        const MaterialBindingCacheDesc& desc) :
        m_Device(rhiDevice),
        m_EndFrameHandle(engineFrame.OnUpdate({ &MaterialBindingCache::ResetFrameCache, this })),
        m_DescriptorTable(rhiDevice.GetResourceAllocator().CreateDescriptorTable(desc.DescriptorPoolDesc)),
        m_DynamicBuffer(rhiDevice, desc.BufferAllocatorDesc)
    {
    }

    void MaterialBindingCache::Bind(
        Rhi::CommandList&        commandList,
        const Shading::Material& material)
    {
        commandList.SetPipelineLayout(*material.GetPipelineLayout()->Unwrap());

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
                *setCache.DescriptorSet.Unwrap(),
                setCache.DynamicBufferOffset == InvalidDynamicBufferOffset ? nullptr : &setCache.DynamicBufferOffset);
        }
    }

    Rhi::DescriptorSet MaterialBindingCache::AllocateSet(
        const Shading::Material& material,
        uint32_t                 setIndex,
        uint32_t                 variableCount)
    {
        return m_DescriptorTable.AllocateSet(*material.GetPipelineLayout()->Unwrap(), setIndex, variableCount);
    }

    //

    void MaterialBindingCache::ResetFrameCache()
    {
        m_DescriptorTable.Reset();
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

        std::vector<nri::DescriptorRangeUpdateDesc> updateDescs;
        for (auto& iter : material.GetResources())
        {
            std::visit(
                VariantVisitor{
                    [&updateDescs](const auto& resource)
                    {
                        updateDescs.emplace_back(nri::DescriptorRangeUpdateDesc{
                            .descriptors   = &resource.View->Unwrap(),
                            .descriptorNum = 1 });
                    } },
                iter->second);
        }

        bool hasMaterialData = bufferSize || !updateDescs.empty();
        if (hasMaterialData)
        {
            setCache.DescriptorSet = AllocateSet(material, CD::c_MaterialData_SetIndex);
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

        if (!updateDescs.empty())
        {
            setCache.DescriptorSet.SetRanges(0, updateDescs);
        }
    }

    const nri::Descriptor* MaterialBindingCache::GetOrCreateConstantBufferDescriptor(
        const BufferAllocator::Handle& handle)
    {
        auto& view = m_DynamicBufferDescriptors[handle.BlockSlot];
        if (!view)
        {
            auto& resourceAllocator = m_Device.get().GetResourceAllocator();

            auto&            buffer = m_DynamicBuffer.GetBuffer(handle.BlockSlot);
            Rhi::BufferRange range(handle.Offset, handle.Size);

            view = buffer.CreateView({ .Range = range });
        }
        return view.Unwrap();
    }
} // namespace Ame::Gfx::Cache
