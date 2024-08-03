#pragma once

#include <Rhi/Wrapper/DeviceWrapper.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    enum class ExecutorType : uint8_t
    {
        Primary, // Executor for the main rendering operations

        Graphics,  // Executor for graphics commands
        Compute,   // Executor for compute commands
        Copy,      // Executor for copy commands
        Resources, // Executor for resource creation and destruction

        Count
    };

    class RhiDeviceImpl : public BaseObject<IRhiDevice>
    {
    public:
        using Base = BaseObject<IRhiDevice>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(
            IID_RhiDevice, Base,
            m_Wrapper.GetFactory(),
            m_Wrapper.GetDevice(),
            m_Wrapper.GetImmediateContext(),
            m_Wrapper.GetWindowWrapper() ? m_Wrapper.GetWindowWrapper()->GetWindow() : nullptr,
            m_Wrapper.GetWindowWrapper() ? m_Wrapper.GetWindowWrapper()->GetSwapchain() : nullptr);

    public:
        RhiDeviceImpl(
            IReferenceCounters*     counters,
            const DeviceCreateDesc& createDesc);

    public:
        bool BeginFrame() override;

        void AdvanceFrame(
            uint32_t syncInterval) override;

        Dg::IEngineFactory* GetFactory() const override;
        Dg::IRenderDevice*  GetRenderDevice() const override;
        Dg::IDeviceContext* GetImmediateContext() const override;
        Window::IWindow*    GetWindow() const override;
        Dg::ISwapChain*     GetSwapchain() const override;

    private:
        DeviceWrapper m_Wrapper;

        uint32_t m_ConcurrentLoopCount;
        uint32_t m_ConcurrentLoopChunkSize;
    };
} // namespace Ame::Rhi