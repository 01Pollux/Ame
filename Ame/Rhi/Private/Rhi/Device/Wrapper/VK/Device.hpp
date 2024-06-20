#pragma once

#include <Rhi/Device/Wrapper/VK/Core.hpp>
#include <Rhi/Device/Wrapper/DeviceWrapper.hpp>

#include <boost/dll.hpp>

namespace Ame::Rhi::VK
{
    class VkDeviceWrapper : public IDeviceWrapper
    {
        using DllType = boost::dll::shared_library;

        static constexpr uint32_t c_MinVulkanVersion = VK_API_VERSION_1_2;

        struct VkInitDesc
        {
            std::vector<const char*> LayerExtensions;
            std::vector<const char*> InstanceExtensions;
            std::vector<const char*> DeviceExtensions;
            std::vector<uint32_t>    QueueFamilies;
            bool                     IsVulkan12 = false;
        };

    public:
        VkDeviceWrapper(
            const DeviceCreateDesc& createDesc);

        ~VkDeviceWrapper() override;

    protected:
        bool Initialize(
            const DeviceCreateDesc& createDesc) override;

    private:
        /// <summary>
        /// Load the Vulkan library.
        /// </summary>
        void LoadVulkanLibrary();

        /// <summary>
        /// Fill the layer extensions.
        /// </summary>
        void FillLayerExtensions(
            const DeviceCreateDesc& createDesc,
            VkInitDesc&             vkInitDesc) const;

        /// <summary>
        /// Create the Vulkan instance.
        /// </summary>
        void CreateVulkanInstance(
            const DeviceCreateDesc& createDesc,
            VkInitDesc&             vkInitDesc);

        /// <summary>
        /// Enable validation layers if needed.
        /// </summary>
        void EnableValidationIfNeeded(
            const DeviceCreateDesc& createDesc);

        /// <summary>
        /// Fill the instance extensions.
        /// </summary>
        void FillInstanceExtensions(
            const DeviceCreateDesc& createDesc,
            VkInitDesc&             vkInitDesc) const;

        /// <summary>
        /// Get the best physical device.
        /// </summary>
        void GetBestPhysicalDevice(
            VkInitDesc& vkInitDesc);

        /// <summary>
        /// Fill the family indices.
        /// </summary>
        void FillFamilyIndices(
            VkInitDesc& vkInitDesc) const;

        /// <summary>
        /// Fill the device extensions.
        /// </summary>
        void FillDeviceExtensions(
            const DeviceCreateDesc& createDesc,
            VkInitDesc&             vkInitDesc) const;

        /// <summary>
        /// Get the logical device
        /// </summary>
        void CreateLogicalDevice(
            const VkInitDesc& vkInitDesc);

        /// <summary>
        /// Create the NRI device.
        /// </summary>
        void CreateNRIDevice(
            const DeviceCreateDesc& createDesc,
            const VkInitDesc&       vkInitDesc);

    private:
        /// <summary>
        /// Callback for debug messages.
        /// </summary>
        static VkBool32 VKAPI_PTR OnDebugMessage(
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
            void*                                       userData);

    private:
        /// <summary>
        /// Check if an layer is supported.
        /// </summary>
        [[nodiscard]] static bool IsLayerSupported(
            std::span<const VkLayerProperties> layers,
            const char*                        layer);

        /// <summary>
        /// Check if an extension is supported.
        /// </summary>
        [[nodiscard]] static bool IsExtensionSupported(
            std::span<const VkExtensionProperties> extensions,
            const char*                            extension);

        /// <summary>
        /// Check if an extension is supported.
        /// </summary>
        [[nodiscard]] static bool IsExtensionSupported(
            std::span<const char* const> extensions,
            const char*                  extension);

    private:
        DllType m_VulkanDll;

        VkAllocationCallbacks m_VkAllocationCallbacks = {};

        PFN_vkGetInstanceProcAddr m_vkGetInstanceProcAddr = nullptr;
        PFN_vkGetDeviceProcAddr   m_vkGetDeviceProcAddr   = nullptr;

        VkInstance               m_VkInstance       = nullptr;
        VkDebugUtilsMessengerEXT m_VkDebugMessenger = nullptr;

        VkPhysicalDevice m_VkPhysicalDevice = nullptr;
        VkDevice         m_VkDevice         = nullptr;
        uint32_t         m_VkVersion        = 0;

        nri::WrapperVKInterface m_NriVk;

        std::vector<uint32_t> m_QueueFamilies;
        bool                  m_IsDeviceAddressSupported = false;
    };
} // namespace Ame::Rhi::VK
