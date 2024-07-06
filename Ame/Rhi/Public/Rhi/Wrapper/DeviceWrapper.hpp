#pragma once

#include <set>

#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/Wrapper/WindowWrapper.hpp>

namespace Ame::Rhi
{
    class DeviceCreateDesc;

    class DeviceWrapper
    {
    public:
        /// <summary>
        /// Create a device wrapper.
        /// </summary>
        [[nodiscard]] static Opt<DeviceWrapper> Create(
            const DeviceCreateDesc& createDesc);

    public:
        DeviceWrapper() = default;
        DeviceWrapper(
            Dg::Ptr<Dg::IEngineFactory> engineFactory,
            Dg::Ptr<Dg::IRenderDevice>  renderDevice,
            Dg::Ptr<Dg::IDeviceContext> deviceContext,
            WindowWrapper               windowWrapper);

    public:
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return m_EngineFactory != nullptr;
        }

    public:
        /// <summary>
        /// Get the engine factory.
        /// </summary>
        [[nodiscard]] Dg::IEngineFactory* GetFactory() const;

        /// <summary>
        /// Get the graphics device.
        /// </summary>
        [[nodiscard]] Dg::IRenderDevice* GetDevice() const;

        /// <summary>
        /// Get the device immediate context.
        /// </summary>
        [[nodiscard]] Dg::IDeviceContext* GetImmediateContext() const;

        /// <summary>
        /// Get the window wrapper.
        /// </summary>
        [[nodiscard]] const WindowWrapper& GetWindowWrapper() const noexcept;

        /// <summary>
        /// Get the window wrapper.
        /// </summary>
        [[nodiscard]] WindowWrapper& GetWindowWrapper() noexcept;

    private:
        /// <summary>
        /// Create a device wrapper for D3D11.
        /// </summary>
        [[nodiscard]] static Opt<DeviceWrapper> CreateImpl(
            const DeviceCreateDesc&      createDesc,
            const DeviceCreateDescD3D11& createDescD3D11);

        /// <summary>
        /// Create a device wrapper for D3D12.
        /// </summary>
        [[nodiscard]] static Opt<DeviceWrapper> CreateImpl(
            const DeviceCreateDesc&      createDesc,
            const DeviceCreateDescD3D12& createDescD3D12);

        /// <summary>
        /// Create a device wrapper for OpenGL.
        /// </summary>
        [[nodiscard]] static Opt<DeviceWrapper> CreateImpl(
            const DeviceCreateDesc&   createDesc,
            const DeviceCreateDescGL& createDescGL);

        /// <summary>
        /// Create a device wrapper for Vulkan.
        /// </summary>
        [[nodiscard]] static Opt<DeviceWrapper> CreateImpl(
            const DeviceCreateDesc&       createDesc,
            const DeviceCreateDescVulkan& createDescVK);

    protected:
        Dg::Ptr<Dg::IEngineFactory> m_EngineFactory;
        Dg::Ptr<Dg::IRenderDevice>  m_RenderDevice;
        Dg::Ptr<Dg::IDeviceContext> m_ImmediateContext;
        WindowWrapper               m_WindowWrapper;
    };
} // namespace Ame::Rhi