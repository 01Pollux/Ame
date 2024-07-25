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
        for (auto& type : createDesc.Types)
        {
            auto deviceWrapper = std::visit(
                VariantVisitor{
                    [&](const auto& deviceDesc)
                    { return CreateImpl(createDesc, deviceDesc); } },
                type);
            if (deviceWrapper)
            {
                return deviceWrapper;
            }
        }

        Log::Rhi().Fatal("Failed to create renderer device");
        return std::nullopt;
    }

    //

    DeviceWrapper::DeviceWrapper(
        Ptr<Dg::IEngineFactory>  engineFactory,
        Ptr<Dg::IRenderDevice>   renderDevice,
        Ptr<Dg::IDeviceContext>  deviceContext,
        UniquePtr<WindowWrapper> windowWrapper) :
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

    const WindowWrapper* DeviceWrapper::GetWindowWrapper() const noexcept
    {
        return m_WindowWrapper.get();
    }

    WindowWrapper* DeviceWrapper::GetWindowWrapper() noexcept
    {
        return m_WindowWrapper.get();
    }
} // namespace Ame::Rhi