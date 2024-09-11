#include <Rhi/Wrapper/DeviceWrapper.Generic.hpp>

#ifdef VULKAN_SUPPORTED
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include <vulkan/vulkan.h>
#endif

namespace Ame::Rhi
{
#ifdef VULKAN_SUPPORTED
    struct DeviceCreateTraitsVk
    {
        using create_struct_type    = DeviceCreateDescVulkan;
        using diligent_create_info  = Dg::EngineVkCreateInfo;
        using diligent_factory_type = Dg::IEngineFactoryVk;

        static const char* GetName()
        {
            return "Vulkan";
        }

        static diligent_create_info GetCreateInfo(const DeviceCreateDesc&, const create_struct_type& createDescDev)
        {
            static VkAllocationCallbacks allocation{
                .pfnAllocation = [](void*, size_t size, size_t alignment, VkSystemAllocationScope) -> void*
                { return mi_aligned_alloc(alignment, size); },
                .pfnReallocation = [](void*, void* ptr, size_t size, size_t, VkSystemAllocationScope) -> void*
                { return mi_realloc(ptr, size); },
                .pfnFree = [](void*, void* ptr) { mi_free(ptr); },
                .pfnInternalAllocation = [](void*, size_t, VkInternalAllocationType, VkSystemAllocationScope) {},
                .pfnInternalFree       = [](void*, size_t, VkInternalAllocationType, VkSystemAllocationScope) {}
            };

            diligent_create_info createInfo;

            createInfo.InstanceLayerCount   = static_cast<uint32_t>(createDescDev.RequiredLayerExtensions.size());
            createInfo.ppInstanceLayerNames = createDescDev.RequiredLayerExtensions.data();

            createInfo.InstanceExtensionCount   = static_cast<uint32_t>(createDescDev.RequiredLayerExtensions.size());
            createInfo.ppInstanceExtensionNames = createDescDev.RequiredInstanceExtensions.data();

            createInfo.DeviceExtensionCount   = static_cast<uint32_t>(createDescDev.RequiredLayerExtensions.size());
            createInfo.ppDeviceExtensionNames = createDescDev.RequiredDeviceExtensions.data();

            createInfo.pDeviceExtensionFeatures = createDescDev.RequiredDeviceFeatures;
            createInfo.pVkAllocator             = &allocation;

            createInfo.IgnoreDebugMessageCount   = static_cast<uint32_t>(createDescDev.IgnoredDebugMessageNames.size());
            createInfo.ppIgnoreDebugMessageNames = createDescDev.IgnoredDebugMessageNames.data();

            for (auto [pool, desc] :
                 { std::pair{ &createInfo.MainDescriptorPoolSize, &createDescDev.MainDescriptorPool },
                   std::pair{ &createInfo.DynamicDescriptorPoolSize, &createDescDev.DynamicDescriptorPool } })
            {
                GetOrDefault(pool->MaxDescriptorSets, desc->MaxDescriptorSets);
                GetOrDefault(pool->NumSeparateSamplerDescriptors, desc->NumSeparateSamplerDescriptors);
                GetOrDefault(pool->NumCombinedSamplerDescriptors, desc->NumCombinedSamplerDescriptors);
                GetOrDefault(pool->NumSampledImageDescriptors, desc->NumSampledImageDescriptors);
                GetOrDefault(pool->NumStorageImageDescriptors, desc->NumStorageImageDescriptors);
                GetOrDefault(pool->NumUniformBufferDescriptors, desc->NumUniformBufferDescriptors);
                GetOrDefault(pool->NumStorageBufferDescriptors, desc->NumStorageBufferDescriptors);
                GetOrDefault(pool->NumUniformTexelBufferDescriptors, desc->NumUniformTexelBufferDescriptors);
                GetOrDefault(pool->NumStorageTexelBufferDescriptors, desc->NumStorageTexelBufferDescriptors);
                GetOrDefault(pool->NumInputAttachmentDescriptors, desc->NumInputAttachmentDescriptors);
                GetOrDefault(pool->NumAccelStructDescriptors, desc->NumAccelStructDescriptors);
            }

            GetOrDefault(createInfo.DeviceLocalMemoryPageSize, createDescDev.DeviceLocalMemoryPageSize);
            GetOrDefault(createInfo.HostVisibleMemoryPageSize, createDescDev.HostVisibleMemoryPageSize);
            GetOrDefault(createInfo.DeviceLocalMemoryReserveSize, createDescDev.DeviceLocalMemoryReserveSize);
            GetOrDefault(createInfo.HostVisibleMemoryReserveSize, createDescDev.HostVisibleMemoryReserveSize);
            GetOrDefault(createInfo.UploadHeapPageSize, createDescDev.UploadHeapPageSize);
            GetOrDefault(createInfo.DynamicHeapSize, createDescDev.DynamicHeapSize);
            GetOrDefault(createInfo.DynamicHeapPageSize, createDescDev.DynamicHeapPageSize);

            GetOrDefault(createInfo.QueryPoolSizes[1], createDescDev.QueryHeapSizes.Occlusion);
            GetOrDefault(createInfo.QueryPoolSizes[2], createDescDev.QueryHeapSizes.BinaryOcclusion);
            GetOrDefault(createInfo.QueryPoolSizes[3], createDescDev.QueryHeapSizes.Timestamp);
            GetOrDefault(createInfo.QueryPoolSizes[4], createDescDev.QueryHeapSizes.PipelineStatistics);
            GetOrDefault(createInfo.QueryPoolSizes[5], createDescDev.QueryHeapSizes.Duration);

            return createInfo;
        }

        static diligent_factory_type* LoadFactory()
        {
            auto getEngineFactoryVK = Dg::LoadGraphicsEngineVk();
            return getEngineFactoryVK ? getEngineFactoryVK() : nullptr;
        }

        static void CreateDeviceAndContext(diligent_factory_type* factory, diligent_create_info createInfo,
                                           Dg::IRenderDevice** renderDevice, Dg::IDeviceContext** deviceContext)
        {
            factory->CreateDeviceAndContextsVk(createInfo, renderDevice, deviceContext);
        }

        static void CreateSwapchain(diligent_factory_type* factory, Dg::IRenderDevice* renderDevice,
                                    Dg::IDeviceContext* deviceContext, const Dg::SwapChainDesc& swapchainDesc,
                                    const Dg::FullScreenModeDesc&, const Dg::NativeWindow&      nativeWindow,
                                    Dg::ISwapChain** swapchain)
        {
            factory->CreateSwapChainVk(renderDevice, deviceContext, swapchainDesc, nativeWindow, swapchain);
        }
    };
#endif

    //

    Opt<DeviceWrapper> DeviceWrapper::CreateImpl(const DeviceCreateDesc&       createDesc,
                                                 const DeviceCreateDescVulkan& createDescDev)
    {
#ifdef VULKAN_SUPPORTED
        return GenericDeviceCreator<DeviceCreateTraitsVk>::Create(createDesc, createDescDev);
#else
        return std::nullopt;
#endif
    }
} // namespace Ame::Rhi