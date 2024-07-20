#include <Rhi/Device/RhiDeviceImpl.hpp>

#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.hpp>
#include <Window/Window.hpp>

#include <Log/Wrapper.hpp>

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

    const char* IRhiDevice::GetGraphicsAPIName(
        bool upperCase)
    {
        return Dg::GetRenderDeviceTypeShortString(GetGraphicsAPI(), upperCase);
    }

    //

    RhiDeviceImpl::RhiDeviceImpl(
        IReferenceCounters*     counters,
        const DeviceCreateDesc& createDesc) :
        Base(counters),
        m_Wrapper(std::move(*DeviceWrapper::Create(createDesc)))
    {
    }

    bool RhiDeviceImpl::BeginFrame()
    {
        if (m_Wrapper.GetWindowWrapper())
        {
            auto window = m_Wrapper.GetWindowWrapper().GetWindow();
            window->ProcessEvents();
            return window->IsRunning();
        }
        return true;
    }

    void RhiDeviceImpl::AdvanceFrame(
        uint32_t syncInterval)
    {
        auto& windowWrapper = m_Wrapper.GetWindowWrapper();
        if (windowWrapper)
        {
            windowWrapper.Present(syncInterval);
        }
        else
        {
            m_Wrapper.GetImmediateContext()->FinishFrame();
        }
    }
} // namespace Ame::Rhi