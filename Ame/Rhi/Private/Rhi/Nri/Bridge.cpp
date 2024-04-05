#include "Bridge.hpp"

#include "../Nri/Nri.hpp"
#include "../NriError.hpp"

namespace Ame::Rhi
{
    /// <summary>
    /// Try to load an nri feature interface.
    /// </summary>
    template<typename Ty>
    static bool TryLoadFeature(
        nri::Device&      RhiDevice,
        const char*       InterfaceName,
        size_t            InterfaceSize,
        UPtr<Ty>&         InterfacePtr,
        DeviceFeatureType Features,
        bool              Supported = true)
    {
        if (Supported)
        {
            if (!InterfacePtr)
            {
                InterfacePtr = std::make_unique<Ty>();
            }

            if (nri::nriGetInterface(RhiDevice, InterfaceName, InterfaceSize, InterfacePtr.get()) != nri::Result::SUCCESS &&
                Features == DeviceFeatureType::Required) [[unlikely]]
            {
                Supported = false;
            }
        }
        else if (Features != DeviceFeatureType::Required)
        {
            Supported = true;
        }
        if (!Supported)
        {
            Log::Rhi().Error("Device does not support {}", InterfaceName);
        }
        return Supported;
    }

    //

    bool NRIBridge::Initialize(
        nri::Device&      RhiDevice,
        DeviceFeatureType SwapchainFeatures,
        DeviceFeatureType RayTracingFeatures,
        DeviceFeatureType MeshShaderFeatures)
    {
        if (!TryLoadFeature(RhiDevice, NRI_INTERFACE(nri::CoreInterface), m_CoreInterface, DeviceFeatureType::Required))
        {
            Shutdown();
            return false;
        }

        ThrowIfFailed(m_CoreInterface->CreateFence(RhiDevice, 0, m_IdleFence), "Failed to create idle fence");

        auto& DevDesc = m_CoreInterface->GetDeviceDesc(RhiDevice);

        if (!TryLoadFeature(RhiDevice, NRI_INTERFACE(nri::SwapChainInterface), m_SwapChainInterface, SwapchainFeatures))
        {
            Shutdown();
            return false;
        }
        if (!TryLoadFeature(RhiDevice, NRI_INTERFACE(nri::MeshShaderInterface), m_MeshShaderInterface, MeshShaderFeatures, DevDesc.isMeshShaderSupported))
        {
            Shutdown();
            return false;
        }
        if (!TryLoadFeature(RhiDevice, NRI_INTERFACE(nri::RayTracingInterface), m_RayTracingInterface, RayTracingFeatures, DevDesc.isRayTracingSupported))
        {
            Shutdown();
            return false;
        }

        return true;
    }

    void NRIBridge::Shutdown()
    {
        if (m_IdleFence)
        {
            m_CoreInterface->DestroyFence(*m_IdleFence);
            m_IdleFence = nullptr;
        }

        m_CoreInterface.reset();
        m_SwapChainInterface.reset();
        m_MeshShaderInterface.reset();
        m_RayTracingInterface.reset();
    }

    //

    void NRIBridge::WaitIdle(
        nri::CommandQueue& CommandQueue)
    {
        nri::FenceSubmitDesc FenceDesc{
            .fence = m_IdleFence,
            .value = ++m_FenceValue
        };

        nri::QueueSubmitDesc SubmitDesc{
            .signalFences   = &FenceDesc,
            .signalFenceNum = 1
        };

        m_CoreInterface->QueueSubmit(CommandQueue, SubmitDesc);
        m_CoreInterface->Wait(*m_IdleFence, m_FenceValue);
    }
} // namespace Ame::Rhi