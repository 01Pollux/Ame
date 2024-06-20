#include <Rhi/Device/Wrapper/D3D12/Device.hpp>
#include <Rhi/Device/Wrapper/VK/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    UPtr<IDeviceWrapper> IDeviceWrapper::Create(
        const DeviceCreateDesc& createDesc,
        DeviceType              deviceType)
    {
        UPtr<IDeviceWrapper> deviceWrapper = nullptr;
        try
        {
            switch (deviceType)
            {
            case DeviceType::DirectX12:
#ifdef AME_PLATFORM_WINDOWS
                deviceWrapper = std::make_unique<D3D12::D3D12DeviceWrapper>(createDesc);
#endif
                break;

            case DeviceType::Vulkan:
                deviceWrapper = std::make_unique<VK::VkDeviceWrapper>(createDesc);
                break;

            default:
                std::unreachable();
            }
        }
        catch (const std::exception& ex)
        {
            Log::Rhi().Error(ex.what());
            return nullptr;
        }

        if (!deviceWrapper->Initialize(createDesc))
        {
            deviceWrapper = nullptr;
        }

        return deviceWrapper;
    }

    IDeviceWrapper::~IDeviceWrapper()
    {
        Shutdown();

        if (m_Device)
        {
            nri::nriDestroyDevice(*m_Device);
            m_Device = nullptr;
        }
    }

    //

    nri::Device& IDeviceWrapper::GetNriDevice() const noexcept
    {
        return *m_Device;
    }

    const NRIBridge& IDeviceWrapper::GetNri() const noexcept
    {
        return m_NRI;
    }

    NRIBridge& IDeviceWrapper::GetNri() noexcept
    {
        return m_NRI;
    }

    std::set<nri::CommandQueue*> IDeviceWrapper::GetQueueSet() noexcept
    {
        return { m_GraphicsQueue, m_ComputeQueue, m_CopyQueue };
    }

    nri::CommandQueue& IDeviceWrapper::GetQueue(
        CommandQueueType queueType) noexcept
    {
        switch (queueType)
        {
        case CommandQueueType::GRAPHICS:
            return *m_GraphicsQueue;
        case CommandQueueType::COMPUTE:
            return *m_ComputeQueue;
        case CommandQueueType::COPY:
            return *m_CopyQueue;
        default:
            std::unreachable();
        }
    }

    nri::CommandQueue& IDeviceWrapper::GetGraphicsQueue() noexcept
    {
        return GetQueue(CommandQueueType::GRAPHICS);
    }

    nri::CommandQueue& IDeviceWrapper::GetComputeQueue() noexcept
    {
        return GetQueue(CommandQueueType::COMPUTE);
    }

    nri::CommandQueue& IDeviceWrapper::GetCopyQueue() noexcept
    {
        return GetQueue(CommandQueueType::COPY);
    }

    //

    IDeviceMemoryAllocator& IDeviceWrapper::GetMemoryAllocator() noexcept
    {
        return *m_MemoryAllocator;
    }

    //

    bool IDeviceWrapper::Initialize(
        const DeviceCreateDesc& createDesc)
    {
        auto swapchainFeatures = createDesc.Window.has_value() ? DeviceFeatureType::Required : DeviceFeatureType::Disabled;

        if (!m_NRI.Initialize(*m_Device, swapchainFeatures, createDesc.MeshShaderFeatures, createDesc.RayTracingFeatures))
        {
            return false;
        }

        if (m_NRI.GetCoreInterface()->GetCommandQueue(*m_Device, CommandQueueType::GRAPHICS, m_GraphicsQueue) != nri::Result::SUCCESS)
        {
            return false;
        }
        if (m_NRI.GetCoreInterface()->GetCommandQueue(*m_Device, CommandQueueType::COMPUTE, m_ComputeQueue) != nri::Result::SUCCESS)
        {
            m_ComputeQueue = m_GraphicsQueue;
        }
        if (m_NRI.GetCoreInterface()->GetCommandQueue(*m_Device, CommandQueueType::COPY, m_CopyQueue) != nri::Result::SUCCESS)
        {
            m_CopyQueue = m_GraphicsQueue;
        }

        return true;
    }

    void IDeviceWrapper::Shutdown()
    {
        m_NRI.Shutdown();
        m_MemoryAllocator.reset();
    }
} // namespace Ame::Rhi