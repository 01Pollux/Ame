#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    // {E5EF1DD2-C925-4B33-984D-E24143CB02E4}
    static const UId IID_RhiDevice = { 0xe5ef1dd2, 0xc925, 0x4b33, { 0x98, 0x4d, 0xe2, 0x41, 0x43, 0xcb, 0x2, 0xe4 } };

    /// <summary>
    /// Can query:
    /// - IID_Object
    /// - IID_RhiDevice
    /// - IID_DeviceContext
    /// - IID_Window
    /// - IID_EngineFactory
    /// - IID_RenderDevice
    /// - IID_Swapchain
    /// </summary>
    class IRhiDevice : public IObject
    {
    public:
        [[nodiscard]] virtual bool AME_METHOD(BeginFrame)() = 0;

        /// <summary>
        /// End the rendering device frame and present if device has window.
        /// </summary>
        virtual void AME_METHOD(AdvanceFrame)(
            uint32_t syncInterval = 1) = 0;

    public:
        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] Dg::RENDER_DEVICE_TYPE GetGraphicsAPI();

        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] const char* GetGraphicsAPIName(
            bool upperCase = false);
    };

    /// <summary>
    /// Create a rendering device.
    /// </summary>
    [[nodiscard]] Ptr<IRhiDevice> AME_METHOD(CreateRhiDevice)(
        const DeviceCreateDesc& desc);
} // namespace Ame::Rhi