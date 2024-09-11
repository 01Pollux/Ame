#include <Rhi/Device/RhiDeviceImpl.hpp>

#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.hpp>
#include <Window/Window.hpp>

namespace Ame::Rhi
{
    Dg::RENDER_DEVICE_TYPE IRhiDevice::GetGraphicsAPI()
    {
        Dg::RENDER_DEVICE_TYPE deviceType = Dg::RENDER_DEVICE_TYPE_UNDEFINED;

        Ptr<Dg::IRenderDevice> device;
        this->QueryInterface(IID_RhiDevice, device.DblPtr<IObject>());
        if (device)
        {
            deviceType = device->GetDeviceInfo().Type;
        }

        return deviceType;
    }

    const char* IRhiDevice::GetGraphicsAPIName(bool upperCase)
    {
        return Dg::GetRenderDeviceTypeShortString(GetGraphicsAPI(), upperCase);
    }

    //

    RhiDeviceImpl::RhiDeviceImpl(IReferenceCounters* counters, const DeviceCreateDesc& createDesc) :
        Base(counters), m_Wrapper(*DeviceWrapper::Create(createDesc)),
        m_CommonRenderPass(AmeCreate(CommonRenderPass, this))
    {
    }

    bool RhiDeviceImpl::BeginFrame()
    {
        auto windowWrapper = m_Wrapper.GetWindowWrapper();
        if (windowWrapper)
        {
            auto window = windowWrapper->GetWindow();
            return window->IsRunning();
        }
        return true;
    }

    void RhiDeviceImpl::AdvanceFrame(uint32_t syncInterval)
    {
        auto windowWrapper = m_Wrapper.GetWindowWrapper();
        if (windowWrapper)
        {
            windowWrapper->Present(syncInterval);
        }
        else
        {
            m_Wrapper.GetImmediateContext()->FinishFrame();
        }
    }

    Dg::IEngineFactory* RhiDeviceImpl::GetFactory() const
    {
        return m_Wrapper.GetFactory();
    }

    Dg::IRenderDevice* RhiDeviceImpl::GetRenderDevice() const
    {
        return m_Wrapper.GetDevice();
    }

    Dg::IDeviceContext* RhiDeviceImpl::GetImmediateContext() const
    {
        return m_Wrapper.GetImmediateContext();
    }

    Window::IWindow* RhiDeviceImpl::GetWindow() const
    {
        auto windowWrapper = m_Wrapper.GetWindowWrapper();
        return windowWrapper ? windowWrapper->GetWindow() : nullptr;
    }

    Dg::ISwapChain* RhiDeviceImpl::GetSwapchain() const
    {
        auto windowWrapper = m_Wrapper.GetWindowWrapper();
        return windowWrapper ? windowWrapper->GetSwapchain() : nullptr;
    }

    CommonRenderPass* RhiDeviceImpl::GetCommonRenderPass() const
    {
        return m_CommonRenderPass;
    }

    Dg::IRenderStateCache* RhiDeviceImpl::GetRenderStateCache() const
    {
        return m_RenderStateCache;
    }
} // namespace Ame::Rhi