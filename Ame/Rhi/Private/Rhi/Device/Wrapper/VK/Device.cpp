#include <stdexcept>

#include <Rhi/Device/Wrapper/VK/Device.hpp>
#include <Rhi/Device/Wrapper/VK/MemoryAllocator.hpp>

#include <Rhi/Device/SprivBinding.hpp>

#include <Rhi/Nri/Allocator.hpp>
#include <Rhi/Nri/Log.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi::VK
{
    VkDeviceWrapper::VkDeviceWrapper(
        const DeviceCreateDesc& createDesc)
    {
        VkInitDesc vkInitDesc;
        LoadVulkanLibrary();

        FillLayerExtensions(createDesc, vkInitDesc);
        FillInstanceExtensions(createDesc, vkInitDesc);
        CreateVulkanInstance(createDesc, vkInitDesc);

        GetBestPhysicalDevice(vkInitDesc);
        EnableValidationIfNeeded(createDesc);

        FillFamilyIndices(vkInitDesc);
        FillDeviceExtensions(createDesc, vkInitDesc);

        CreateLogicalDevice(vkInitDesc);
        CreateNRIDevice(createDesc, vkInitDesc);
    }

    VkDeviceWrapper::~VkDeviceWrapper()
    {
        Shutdown();

        if (m_VkDevice)
        {
            auto vkDestroyDevice = std::bit_cast<PFN_vkDestroyDevice>(
                m_vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDevice"));
            vkDestroyDevice(m_VkDevice, &m_VkAllocationCallbacks);
        }

        if (m_VkDebugMessenger)
        {
            auto vkDestroyDebugUtilsMessengerEXT = std::bit_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                m_vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT"));
            vkDestroyDebugUtilsMessengerEXT(m_VkInstance, m_VkDebugMessenger, &m_VkAllocationCallbacks);
        }

        if (m_VkInstance)
        {
            auto vkDestroyInstance = std::bit_cast<PFN_vkDestroyInstance>(
                m_vkGetInstanceProcAddr(m_VkInstance, "vkDestroyInstance"));
            vkDestroyInstance(m_VkInstance, &m_VkAllocationCallbacks);
        }
    }

    //

    bool VkDeviceWrapper::Initialize(
        const DeviceCreateDesc& createDesc)
    {
        if (!IDeviceWrapper::Initialize(createDesc))
        {
            return false;
        }

        if (nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::WrapperVKInterface), &m_NriVk) != nri::Result::SUCCESS)
        {
            return false;
        }

        VkDeviceMemoryAllocatorDesc memoryAllocatorDesc{
            .NriDevice = *m_Device,
            .NriCore   = *m_NRI.GetCoreInterface(),
            .NriVk     = m_NriVk,

            .MemoryDesc          = createDesc.MemoryDesc,
            .AllocationCallbacks = m_VkAllocationCallbacks,

            .GetInstanceProcAddrVk = m_vkGetInstanceProcAddr,
            .GetDeviceProcAddrVk   = m_vkGetDeviceProcAddr,

            .Instance       = m_VkInstance,
            .PhysicalDevice = m_VkPhysicalDevice,
            .LogicalDevice  = m_VkDevice,

            .QueueFamilies            = m_QueueFamilies,
            .VulkanVersion            = m_VkVersion,
            .IsDeviceAddressSupported = m_IsDeviceAddressSupported
        };
        m_MemoryAllocator = std::make_unique<VKDeviceMemoryAllocator>(memoryAllocatorDesc);
        return true;
    }

    //

    void VkDeviceWrapper::LoadVulkanLibrary()
    {
        auto dllLoadFlags = boost::dll::load_mode::append_decorations |
                            boost::dll::load_mode::search_system_folders;

        try
        {
            m_VulkanDll = DllType("vulkan-1", dllLoadFlags);
        }
        catch (...)
        {
            m_VulkanDll = DllType("vulkan", dllLoadFlags);
        }

        m_vkGetInstanceProcAddr = m_VulkanDll.get<std::remove_pointer_t<decltype(m_vkGetInstanceProcAddr)>>("vkGetInstanceProcAddr");
        m_vkGetDeviceProcAddr   = m_VulkanDll.get<std::remove_pointer_t<decltype(m_vkGetDeviceProcAddr)>>("vkGetDeviceProcAddr");

        m_VkAllocationCallbacks = {
            .pfnAllocation = [](void*, size_t size, size_t alignment, VkSystemAllocationScope) -> void*
            { return mi_aligned_alloc(alignment, size); },
            .pfnReallocation    = [](void*, void* ptr, size_t size, size_t, VkSystemAllocationScope) -> void*
            { return mi_realloc(ptr, size); },
            .pfnFree            = [](void*, void* ptr)
            { mi_free(ptr); },
            .pfnInternalAllocation = [](void*, size_t, VkInternalAllocationType, VkSystemAllocationScope) {},
            .pfnInternalFree    = [](void*, size_t, VkInternalAllocationType, VkSystemAllocationScope) {}
        };
    }

    //

    // https://github.com/NVIDIAGameWorks/NRI
    void VkDeviceWrapper::FillLayerExtensions(
        const DeviceCreateDesc& createDesc,
        VkInitDesc&             vkInitDesc) const
    {
        vkInitDesc.LayerExtensions = createDesc.RequiredLayerExtensions | std::ranges::to<std::vector>();

        auto vkEnumerateInstanceLayerProperties = std::bit_cast<PFN_vkEnumerateInstanceLayerProperties>(
            m_vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> supportedLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, supportedLayers.data());

#ifndef AME_DIST
        if (createDesc.EnableApiValidationLayer)
        {
            vkInitDesc.LayerExtensions.push_back("VK_LAYER_KHRONOS_validation");
        }
#endif

        if (Log::Rhi().GetLevel() <= Log::LogLevel::Trace)
        {
            Log::Rhi().Trace("Supported vulkan layers:");
            for (const auto& props : supportedLayers)
            {
                Log::Rhi().Trace("\t\t{} (v{})", props.layerName, props.specVersion);
            }
        }

        for (auto it = vkInitDesc.LayerExtensions.rbegin(); it != vkInitDesc.LayerExtensions.rend(); ++it)
        {
            if (!IsLayerSupported(supportedLayers, *it))
            {
                vkInitDesc.LayerExtensions.erase(it.base() - 1);
            }
        }
    }

    //

    void VkDeviceWrapper::CreateVulkanInstance(
        const DeviceCreateDesc& createDesc,
        VkInitDesc&             vkInitDesc)
    {
        VkInstanceCreateFlags instanceFlags{};

#ifdef __APPLE__
        instanceFlags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

#ifndef AME_DIST
        constexpr VkValidationFeatureEnableEXT enabledValidationFeatures[] = {
            VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
        };

        VkValidationFeaturesEXT validationFeatures{
            .sType                         = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
            .enabledValidationFeatureCount = static_cast<uint32_t>(std::size(enabledValidationFeatures)),
            .pEnabledValidationFeatures    = enabledValidationFeatures
        };
#endif

        auto vkEnumerateInstanceVersion = std::bit_cast<PFN_vkEnumerateInstanceVersion>(
            m_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"));
        vkEnumerateInstanceVersion(&m_VkVersion);

        if (m_VkVersion < c_MinVulkanVersion)
        {
            throw std::runtime_error("Incompatible Vulkan driver version, update your GPU drivers.");
        }

        VkApplicationInfo applicationInfo{
            .sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = m_VkVersion
        };

        VkInstanceCreateInfo instanceCreateInfo{
            .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .flags                   = instanceFlags,
            .pApplicationInfo        = &applicationInfo,
            .enabledLayerCount       = static_cast<uint32_t>(vkInitDesc.LayerExtensions.size()),
            .ppEnabledLayerNames     = vkInitDesc.LayerExtensions.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(vkInitDesc.InstanceExtensions.size()),
            .ppEnabledExtensionNames = vkInitDesc.InstanceExtensions.data()
        };

#ifndef AME_DIST
        if (createDesc.EnableApiValidationLayer)
        {
            instanceCreateInfo.pNext = &validationFeatures;
        }
#endif

        auto vkCreateInstance = std::bit_cast<PFN_vkCreateInstance>(
            m_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance"));
        ThrowIfVkFailed(
            vkCreateInstance(&instanceCreateInfo, &m_VkAllocationCallbacks, &m_VkInstance),
            "Failed to create Vulkan instance");
    }

    //

    void VkDeviceWrapper::EnableValidationIfNeeded(
        const DeviceCreateDesc& createDesc)
    {
#ifndef AME_DIST
        if (createDesc.EnableApiValidationLayer) [[likely]]
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo{
                .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = &OnDebugMessage,
            };

            auto vkCreateDebugUtilsMessengerEXT = std::bit_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                m_vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT"));
            ThrowIfVkFailed(
                vkCreateDebugUtilsMessengerEXT(m_VkInstance, &createInfo, &m_VkAllocationCallbacks, &m_VkDebugMessenger),
                "Failed to create debug messenger");
        }
#endif
    }

    //

    // https://github.com/NVIDIAGameWorks/NRI
    void VkDeviceWrapper::FillInstanceExtensions(
        const DeviceCreateDesc& createDesc,
        VkInitDesc&             vkInitDesc) const
    {
        vkInitDesc.InstanceExtensions = createDesc.RequiredInstanceExtensions | std::ranges::to<std::vector>();

        auto vkEnumerateInstanceExtensionProperties = std::bit_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkEnumerateInstanceExtensionProperties"));

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> supportedExts(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExts.data());

        if (Log::Rhi().GetLevel() <= Log::LogLevel::Trace)
        {
            Log::Rhi().Trace("Supported vulkan instance extensions:");
            for (const auto& props : supportedExts)
            {
                Log::Rhi().Trace("\t\t{} (v{})", props.extensionName, props.specVersion);
            }
        }

        for (auto extension : {
                 VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
#ifdef __APPLE__
                 VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                 VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
#endif
                 VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
                 VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
                 VK_EXT_METAL_SURFACE_EXTENSION_NAME,
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
                 VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
                 VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif
                 VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
                 VK_EXT_DEBUG_UTILS_EXTENSION_NAME })
        {
            if (IsExtensionSupported(supportedExts, extension))
            {
                vkInitDesc.InstanceExtensions.push_back(extension);
            }
        }
    }

    //

    void VkDeviceWrapper::GetBestPhysicalDevice(
        VkInitDesc& vkInitDesc)
    {
        auto vkEnumeratePhysicalDevices = std::bit_cast<PFN_vkEnumeratePhysicalDevices>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkEnumeratePhysicalDevices"));
        auto vkGetPhysicalDeviceProperties = std::bit_cast<PFN_vkGetPhysicalDeviceProperties>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkGetPhysicalDeviceProperties"));

        uint32_t physicalDeviceCount = 0;
        ThrowIfVkFailed(
            vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, nullptr),
            "Failed to enumerate physical devices");

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        ThrowIfVkFailed(
            vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, physicalDevices.data()),
            "Failed to enumerate physical devices");

        VkPhysicalDevice bestPhysicalDevice = VK_NULL_HANDLE;
        uint32_t         bestScore          = 0;
        uint32_t         bestisVulkan12     = 0;

        for (const auto& physicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevice, &props);

            uint32_t major = VK_VERSION_MAJOR(props.apiVersion);
            uint32_t minor = VK_VERSION_MINOR(props.apiVersion);

            if (props.apiVersion < c_MinVulkanVersion)
            {
                continue;
            }

            bool     isVulkan12 = props.apiVersion == VK_API_VERSION_1_2;
            uint32_t score      = minor + major * 10;
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 1000;
            }

            if (score > bestScore)
            {
                bestScore          = score;
                bestPhysicalDevice = physicalDevice;
                bestisVulkan12     = isVulkan12;
            }
        }

        m_VkPhysicalDevice    = bestPhysicalDevice;
        vkInitDesc.IsVulkan12 = bestisVulkan12;
    }

    //

    // https://github.com/NVIDIAGameWorks/NRI
    void VkDeviceWrapper::FillFamilyIndices(
        VkInitDesc& vkInitDesc) const
    {
        auto vkGetPhysicalDeviceQueueFamilyProperties = std::bit_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkGetPhysicalDeviceQueueFamilyProperties"));

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        std::array<uint8_t, std::to_underlying(CommandQueueType::MAX_NUM)> scores{};
        std::array<uint8_t, std::to_underlying(CommandQueueType::MAX_NUM)> familyIndices;

        constexpr uint8_t invalidIndex = std::numeric_limits<uint8_t>::max();
        familyIndices.fill(invalidIndex);

        for (uint8_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            auto& properties = queueFamilyProperties[i];

            VkQueueFlags flags = properties.queueFlags;
            uint8_t      score;

            bool graphics    = flags & VK_QUEUE_GRAPHICS_BIT;
            bool compute     = flags & VK_QUEUE_COMPUTE_BIT;
            bool copy        = flags & VK_QUEUE_TRANSFER_BIT;
            bool sparse      = flags & VK_QUEUE_SPARSE_BINDING_BIT;
            bool protect     = flags & VK_QUEUE_PROTECTED_BIT;
            bool video       = flags & (VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
            bool opticalFlow = flags & VK_QUEUE_OPTICAL_FLOW_BIT_NV;
            bool taken       = false;

            // Scores
            score = (graphics ? 100 : 0) + (compute ? 10 : 0) + (copy ? 10 : 0) + (sparse ? 5 : 0) +
                    (opticalFlow ? 2 : 0) + (video ? 1 : 0) + (protect ? 1 : 0);
            if (!taken && graphics && score > scores[(uint32_t)CommandQueueType::GRAPHICS])
            {
                familyIndices[(uint32_t)CommandQueueType::GRAPHICS] = i;
                scores[(uint32_t)CommandQueueType::GRAPHICS]        = score;
                taken                                               = true;
            }

            score = (!graphics ? 10 : 0) + (compute ? 100 : 0) + (!copy ? 10 : 0) + (sparse ? 5 : 0) +
                    (opticalFlow ? 2 : 0) + (video ? 1 : 0) + (protect ? 1 : 0);
            if (!taken && compute && score > scores[(uint32_t)CommandQueueType::COMPUTE])
            {
                familyIndices[(uint32_t)CommandQueueType::COMPUTE] = i;
                scores[(uint32_t)CommandQueueType::COMPUTE]        = score;
                taken                                              = true;
            }

            score = (!graphics ? 10 : 0) + (!compute ? 10 : 0) + (copy ? 100 : 0) + (sparse ? 5 : 0) +
                    (opticalFlow ? 2 : 0) + (video ? 1 : 0) + (protect ? 1 : 0);
            if (!taken && copy && score > scores[(uint32_t)CommandQueueType::COPY])
            {
                familyIndices[(uint32_t)CommandQueueType::COPY] = i;
                scores[(uint32_t)CommandQueueType::COPY]        = score;
                taken                                           = true;
            }
        }

        vkInitDesc.QueueFamilies =
            familyIndices |
            std::views::filter([invalidIndex](uint8_t index)
                               { return index != invalidIndex; }) |
            std::views::transform([](uint8_t i) -> uint32_t
                                  { return i; }) |
            std::ranges::to<std::vector>();
    }

    //

    // https://github.com/NVIDIAGameWorks/NRI
    void VkDeviceWrapper::FillDeviceExtensions(
        const DeviceCreateDesc& createDesc,
        VkInitDesc&             vkInitDesc) const
    {
        vkInitDesc.DeviceExtensions = createDesc.RequiredDeviceExtensions | std::ranges::to<std::vector>();

        auto vkEnumerateDeviceExtensionProperties = std::bit_cast<PFN_vkEnumerateDeviceExtensionProperties>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkEnumerateDeviceExtensionProperties"));

        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> supportedExts(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &extensionCount, supportedExts.data());

        if (Log::Rhi().GetLevel() <= Log::LogLevel::Trace)
        {
            Log::Rhi().Trace("Supported vulkan device extensions:");
            for (const auto& props : supportedExts)
            {
                Log::Rhi().Trace("\t\t{} (v{})", props.extensionName, props.specVersion);
            }
        }

        // Mandatory
        vkInitDesc.DeviceExtensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

        bool rayTracingOn = createDesc.RayTracingFeatures != DeviceFeatureType::Disabled;
        for (auto [extension, enabled] : {
#ifdef __APPLE__
                 std::pair{ VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, true },
#endif
                 std::pair{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_PRESENT_ID_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_PRESENT_WAIT_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_MAINTENANCE_5_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME, true },
                 std::pair{ VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, rayTracingOn },
                 std::pair{ VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, rayTracingOn },
                 std::pair{ VK_KHR_RAY_QUERY_EXTENSION_NAME, rayTracingOn },
                 std::pair{ VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME, rayTracingOn },
                 std::pair{ VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, rayTracingOn },
                 std::pair{ VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME, true },
                 std::pair{ VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, true },
                 std::pair{ VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME, true },
                 std::pair{ VK_EXT_MESH_SHADER_EXTENSION_NAME, true },
                 std::pair{ VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, true },
                 std::pair{ VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME, true },
                 std::pair{ VK_NV_LOW_LATENCY_2_EXTENSION_NAME, true } })
        {
            if (IsExtensionSupported(supportedExts, extension) && enabled)
            {
                vkInitDesc.DeviceExtensions.push_back(extension);
            }
        }
    }

    //

    void VkDeviceWrapper::CreateLogicalDevice(
        const VkInitDesc& vkInitDesc)
    {
        auto vkGetPhysicalDeviceFeatures2 = std::bit_cast<PFN_vkGetPhysicalDeviceFeatures2>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkGetPhysicalDeviceFeatures2"));

        auto vkCreateDevice = std::bit_cast<PFN_vkCreateDevice>(
            m_vkGetInstanceProcAddr(m_VkInstance, "vkCreateDevice"));

        //

        VkPhysicalDeviceFeatures2 deviceFeatures2{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        };

        //

        void** lastFeature   = &deviceFeatures2.pNext;
        auto   appendFeature = [&](auto& feature)
        {
            *lastFeature = &feature;
            lastFeature  = &feature.pNext;
        };

        auto appendFeatureIfSupported = [&](const char* extension, auto& feature)
        {
            if (IsExtensionSupported(vkInitDesc.DeviceExtensions, extension))
            {
                appendFeature(feature);
            }
        };

        //

        VkPhysicalDeviceVulkan13Features featuresVulkan13{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        if (!vkInitDesc.IsVulkan12)
        {
            appendFeature(featuresVulkan13);
        }

        VkPhysicalDeviceVulkan12Features featuresVulkan12{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        appendFeature(featuresVulkan12);

        VkPhysicalDeviceVulkan11Features featuresVulkan11{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES };
        appendFeature(featuresVulkan11);

        //

#ifdef __APPLE__
        VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubsetFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, portabilitySubsetFeatures);
#endif

        VkPhysicalDeviceMaintenance5FeaturesKHR maintenance5Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_MAINTENANCE_5_EXTENSION_NAME, maintenance5Features);

        VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME, shadingRateFeatures);

        VkPhysicalDevicePresentIdFeaturesKHR presentIdFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_PRESENT_ID_EXTENSION_NAME, presentIdFeatures);

        VkPhysicalDevicePresentWaitFeaturesKHR presentWaitFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_PRESENT_WAIT_EXTENSION_NAME, presentWaitFeatures);

        VkPhysicalDeviceLineRasterizationFeaturesEXT lineRasterizationFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_LINE_RASTERIZATION_FEATURES_EXT };
        appendFeatureIfSupported(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME, lineRasterizationFeatures);

        VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT };
        appendFeatureIfSupported(VK_EXT_MESH_SHADER_EXTENSION_NAME, meshShaderFeatures);

        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, accelerationStructureFeatures);

        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingPipelineFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, rayTracingPipelineFeatures);

        VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_RAY_QUERY_EXTENSION_NAME, rayQueryFeatures);

        VkPhysicalDeviceRayTracingMaintenance1FeaturesKHR rayTracingMaintenanceFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_MAINTENANCE_1_FEATURES_KHR };
        appendFeatureIfSupported(VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME, rayTracingMaintenanceFeatures);

        VkPhysicalDeviceOpacityMicromapFeaturesEXT micromapFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT };
        appendFeatureIfSupported(VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, micromapFeatures);

        VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shaderAtomicFloatFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT };
        appendFeatureIfSupported(VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME, shaderAtomicFloatFeatures);

        VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT shaderAtomicFloat2Features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT };
        appendFeatureIfSupported(VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME, shaderAtomicFloat2Features);

        //

        vkGetPhysicalDeviceFeatures2(m_VkPhysicalDevice, &deviceFeatures2);

        //

        m_IsDeviceAddressSupported = featuresVulkan12.bufferDeviceAddress;

        //

        // Create device
        std::vector<VkDeviceQueueCreateInfo> queues;
        float                                priorities = 1.0f;
        for (uint32_t queueFamily : vkInitDesc.QueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = queueFamily,
                .queueCount       = 1,
                .pQueuePriorities = &priorities
            };
            queues.emplace_back(queueCreateInfo);
        }

        VkDeviceCreateInfo deviceCreateInfo{
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = &deviceFeatures2,
            .queueCreateInfoCount    = static_cast<uint32_t>(queues.size()),
            .pQueueCreateInfos       = queues.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(vkInitDesc.DeviceExtensions.size()),
            .ppEnabledExtensionNames = vkInitDesc.DeviceExtensions.data()
        };

        ThrowIfVkFailed(
            vkCreateDevice(m_VkPhysicalDevice, &deviceCreateInfo, &m_VkAllocationCallbacks, &m_VkDevice),
            "Failed to create logical device");
    }

    //

    void VkDeviceWrapper::CreateNRIDevice(
        const DeviceCreateDesc& createDesc,
        const VkInitDesc&       vkInitDesc)
    {
        m_QueueFamilies = vkInitDesc.QueueFamilies;

        nri::DeviceCreationVKDesc nriDeviceDesc{
            .callbackInterface{
                .MessageCallback = NriLogCallbackInterface::MessageCallback,
                .AbortExecution  = NriLogCallbackInterface::AbortExecution },
            .memoryAllocatorInterface{
                .Allocate   = NriAllocatorCallbackInterface::Allocate,
                .Reallocate = NriAllocatorCallbackInterface::Reallocate,
                .Free       = NriAllocatorCallbackInterface::Free },
            .spirvBindingOffsets = Rhi::c_DefaultSpirvBindingOffset,
            .enabledExtensions{
                .instanceExtensions   = vkInitDesc.InstanceExtensions.data(),
                .instanceExtensionNum = static_cast<uint32_t>(vkInitDesc.InstanceExtensions.size()),
                .deviceExtensions     = vkInitDesc.DeviceExtensions.data(),
                .deviceExtensionNum   = static_cast<uint32_t>(vkInitDesc.DeviceExtensions.size()) },
            .vkInstance          = std::bit_cast<nri::NRIVkInstance>(m_VkInstance),
            .vkDevice            = std::bit_cast<nri::NRIVkInstance>(m_VkDevice),
            .vkPhysicalDevice    = std::bit_cast<nri::NRIVkInstance>(m_VkPhysicalDevice),
            .queueFamilyIndices  = vkInitDesc.QueueFamilies.data(),
            .queueFamilyIndexNum = static_cast<uint32_t>(vkInitDesc.QueueFamilies.size()),
        };

        if (nri::nriCreateDeviceFromVkDevice(nriDeviceDesc, m_Device) != nri::Result::SUCCESS)
        {
            throw std::runtime_error("Failed to create NRI device.");
        }
    }

    //

    VkBool32 VKAPI_PTR VkDeviceWrapper::OnDebugMessage(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void*)
    {
        auto          logLevel     = Log::Rhi().GetLevel();
        Log::LogLevel messageLevel = Log::LogLevel::Info;

        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            messageLevel = Log::LogLevel::Trace;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            messageLevel = Log::LogLevel::Info;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            messageLevel = Log::LogLevel::Warning;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            messageLevel = Log::LogLevel::Error;
            break;
        }

        if (logLevel >= messageLevel)
        {
            return VK_FALSE;
        }

        Log::Rhi().Log(messageLevel, "[Vulkan] {}", callbackData->pMessage);
        return VK_FALSE;
    }

    //

    bool VkDeviceWrapper::IsLayerSupported(
        std::span<const VkLayerProperties> layers,
        const char*                        layer)
    {
        for (const auto& props : layers)
        {
            if (std::strcmp(props.layerName, layer) == 0)
            {
                return true;
            }
        }

        return false;
    }

    bool VkDeviceWrapper::IsExtensionSupported(
        std::span<const VkExtensionProperties> extensions,
        const char*                            extension)
    {
        for (const auto& props : extensions)
        {
            if (std::strcmp(props.extensionName, extension) == 0)
            {
                return true;
            }
        }

        return false;
    }

    bool VkDeviceWrapper::IsExtensionSupported(
        std::span<const char* const> extensions,
        const char*                  extension)
    {
        for (const auto& name : extensions)
        {
            if (std::strcmp(name, extension) == 0)
            {
                return true;
            }
        }

        return false;
    }
} // namespace Ame::Rhi::VK
