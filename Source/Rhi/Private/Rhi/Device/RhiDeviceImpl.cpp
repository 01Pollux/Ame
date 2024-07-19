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

    void AME_CALL_TYPE RhiDeviceImpl::QueryInterface(
        const UId& iid,
        IObject**  outObject)
    {
        if (iid == IID_Unknown || iid == IID_RhiDevice)
        {
            *outObject = this;
            (*outObject)->AddRef();
        }
        else
        {
            auto& windowWrapper = m_Wrapper.GetWindowWrapper();

            IObject* subObjects[]{
                m_Wrapper.GetDevice(),
                m_Wrapper.GetImmediateContext(),
                windowWrapper ? windowWrapper.GetWindow() : nullptr,
                windowWrapper ? windowWrapper.GetSwapchain() : nullptr,
            };

            for (auto subObject : subObjects)
            {
                if (subObject)
                {
                    subObject->QueryInterface(iid, outObject);
                    if (*outObject)
                    {
                        return;
                    }
                }
            }

            Base::QueryInterface(iid, outObject);
        }
    }

    //

    RhiDeviceImpl::RhiDeviceImpl(
        IReferenceCounters*     counters,
        const DeviceCreateDesc& createDesc) :
        Base(counters),
        m_Wrapper(std::move(*DeviceWrapper::Create(createDesc)))
    {
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