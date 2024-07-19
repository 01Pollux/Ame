#include <Rhi/Wrapper/DeviceWrapper.hpp>
#include <Core/Enum.hpp>

#include <Log/Wrapper.hpp>

// TODO:
// Add MSAA swapchain support
// Add auxiliary texture gl texture for swapchain support, since we can't use the swapchain texture directly
namespace Ame::Rhi
{
    Opt<DeviceWrapper> DeviceWrapper::Create(
        const DeviceCreateDesc& createDesc)
    {
        Opt<DeviceWrapper> deviceWrapper;
        for (auto& type : createDesc.Types)
        {
            std::visit(
                VariantVisitor{
                    [&](const auto& deviceDesc)
                    { deviceWrapper = CreateImpl(createDesc, deviceDesc); } },
                type);
            if (deviceWrapper)
            {
                break;
            }
        }
        return deviceWrapper;
    }

    //

    DeviceWrapper::DeviceWrapper(
        Ptr<Dg::IEngineFactory> engineFactory,
        Ptr<Dg::IRenderDevice>  renderDevice,
        Ptr<Dg::IDeviceContext> deviceContext,
        WindowWrapper               windowWrapper) :
        m_EngineFactory(std::move(engineFactory)),
        m_RenderDevice(std::move(renderDevice)),
        m_ImmediateContext(std::move(deviceContext)),
        m_WindowWrapper(std::move(windowWrapper))
    {
    }

    DeviceWrapper::~DeviceWrapper()
    {
        if (m_ImmediateContext)
        {
            m_ImmediateContext->Flush();
        }
    }

    //

    Dg::IEngineFactory* DeviceWrapper::GetFactory() const
    {
        return m_EngineFactory.RawPtr();
    }

    Dg::IRenderDevice* DeviceWrapper::GetDevice() const
    {
        return m_RenderDevice.RawPtr();
    }

    Dg::IDeviceContext* DeviceWrapper::GetImmediateContext() const
    {
        return m_ImmediateContext.RawPtr();
    }

    const WindowWrapper& DeviceWrapper::GetWindowWrapper() const noexcept
    {
        return m_WindowWrapper;
    }

    WindowWrapper& DeviceWrapper::GetWindowWrapper() noexcept
    {
        return m_WindowWrapper;
    }
} // namespace Ame::Rhi