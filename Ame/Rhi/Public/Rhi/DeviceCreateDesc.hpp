#pragma once

#include <Rhi/Core.hpp>
#include <Window/Desc.hpp>
#include <concurrencpp/concurrencpp.h>

namespace Ame::Rhi
{
    struct AdapterDesc : nri::AdapterDesc
    {
        [[nodiscard]] operator bool() const noexcept
        {
            return vendor != nri::Vendor::UNKNOWN;
        }
    };

    //

    enum class DeviceType : uint8_t
    {
        Auto,

        Vulkan,
        DirectX12,

        Count
    };

    struct RhiWindowDesc
    {
        Windowing::WindowDesc Window;

        /// <summary>
        /// The swapchain format to use.
        /// </summary>
        SwapchainFormat SwapChainFormat = SwapchainFormat::BT709_G22_8BIT;

        /// <summary>
        /// Number of backbuffers.
        /// </summary>
        uint32_t BackbufferCount = 3;
    };

    struct DeviceCreateDesc
    {
        /// <summary>
        /// The adapter to use for the device.
        /// </summary>
        AdapterDesc Adapter;

        /// <summary>
        /// A headless device is a device that does not create a window.
        /// </summary>
        std::optional<RhiWindowDesc> Window;

        /// <summary>
        /// VK only.
        /// Required instance extensions.
        /// </summary>
        std::span<const char*> RequiredInstanceExtensions;

        /// <summary>
        /// VK only.
        /// Required device extensions.
        /// </summary>
        std::span<const char*> RequiredDeviceExtensions;

        /// <summary>
        /// Number of frames in flight.
        /// </summary>
        uint32_t FramesInFlight = 3;

        /// <summary>
        /// The device type to use.
        /// </summary>
        DeviceType Type = DeviceType::Auto;

        /// <summary>
        /// Toggle ray tracing support.
        /// </summary>
        DeviceFeatureType RayTracingFeatures = DeviceFeatureType::Disabled;

        /// <summary>
        /// Toggle mesh shader support.
        /// </summary>
        DeviceFeatureType MeshShaderFeatures = DeviceFeatureType::Disabled;

        /// <summary>
        /// Ignored for dist builds.
        /// </summary>
        bool EnableApiValidationLayer : 1 = false;

        /// <summary>
        /// Enable vsync by default.
        /// </summary>
        bool EnableVSync : 1 = true;

    public:
        [[nodiscard]] bool IsHeadless() const
        {
            return !Window.has_value();
        }

        /// <summary>
        /// Set the adapter to use for the device.
        /// </summary>
        void SetFirstAdapter();

        /// <summary>
        /// Enumerate all available adapters.
        /// </summary>
        [[nodiscard]] static Co::generator<AdapterDesc> EnumerateAdapters();
    };

    using DeviceDesc = nri::DeviceDesc;
} // namespace Ame::Rhi