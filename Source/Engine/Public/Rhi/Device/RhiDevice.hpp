#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Window
{
    class IWindow;
} // namespace Ame::Window

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    // {E5EF1DD2-C925-4B33-984D-E24143CB02E4}
    inline constexpr UId IID_RhiDevice{ 0xe5ef1dd2, 0xc925, 0x4b33, { 0x98, 0x4d, 0xe2, 0x41, 0x43, 0xcb, 0x2, 0xe4 } };

    /// <summary>
    /// Can query:
    /// - IID_Object
    /// - IID_RhiDevice
    /// - IID_DeviceContext
    /// - IID_Window
    /// - IID_EngineFactory
    /// - IID_RenderDevice
    /// - IID_Swapchain
    /// - IID_CommonRenderPass
    /// - IID_RenderStateCache
    /// </summary>
    class IRhiDevice : public IObject
    {
    public:
        [[nodiscard]] virtual bool BeginFrame() = 0;

        /// <summary>
        /// End the rendering device frame and present if device has window.
        /// </summary>
        virtual void AdvanceFrame(uint32_t syncInterval = 1) = 0;

    public:
        [[nodiscard]] virtual Dg::IEngineFactory*    GetFactory() const          = 0;
        [[nodiscard]] virtual Dg::IRenderDevice*     GetRenderDevice() const     = 0;
        [[nodiscard]] virtual Dg::IDeviceContext*    GetImmediateContext() const = 0;
        [[nodiscard]] virtual Window::IWindow*       GetWindow() const    = 0; // may be null if there is no window
        [[nodiscard]] virtual Dg::ISwapChain*        GetSwapchain() const = 0; // may be null if there is no window
        [[nodiscard]] virtual CommonRenderPass*      GetCommonRenderPass() const = 0;
        [[nodiscard]] virtual Dg::IRenderStateCache* GetRenderStateCache()
            const = 0; // may be null if the device was not created with a render state cache

    public:
        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] Dg::RENDER_DEVICE_TYPE GetGraphicsAPI();

        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] const char* GetGraphicsAPIName(bool upperCase = false);
    };

    /// <summary>
    /// Create a rendering device.
    /// </summary>
    [[nodiscard]] AME_ENGINE_API Ptr<IRhiDevice> CreateRhiDevice(const DeviceCreateDesc& desc);
} // namespace Ame::Rhi