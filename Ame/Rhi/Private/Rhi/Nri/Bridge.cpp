#include "Bridge.hpp"
#include <NRIDescs.h>

#include "../NriError.hpp"

namespace Ame::Rhi
{
    /// <summary>
    /// Log a missing feature.
    /// </summary>
    static void MissingFeature(const char* FeatureName)
    {
        Log::Rhi().Error("Device does not support {}", FeatureName);
    }

    /// <summary>
    /// Try to load an nri feature interface.
    /// </summary>
    template<typename Ty>
    static bool TryLoadFeature(
        nri::Device&      RhiDevice,
        UPtr<Ty>&         InterfacePtr,
        DeviceFeatureType Features,
        const char*       FeatureName,
        bool              Supported = true)
    {
        if (Features != DeviceFeatureType::Disabled) [[likely]]
        {
            if (Supported)
            {
                if (!InterfacePtr)
                {
                    InterfacePtr = std::make_unique<Ty>();
                }

                if (nri::nriGetInterface(RhiDevice, NRI_INTERFACE(Ty), InterfacePtr.get()) != nri::Result::SUCCESS &&
                    Features == DeviceFeatureType::Required) [[unlikely]]
                {
                    Supported = false;
                }
            }
            else if (Features != DeviceFeatureType::Required)
            {
                Supported = false;
            }
        }
        if (!Supported)
        {
            MissingFeature(FeatureName);
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
        if (!TryLoadFeature(RhiDevice, m_CoreInterface, DeviceFeatureType::Required, "CoreInterface"))
        {
            Shutdown();
            return false;
        }

        ThrowIfFailed(m_CoreInterface->CreateFence(RhiDevice, 0, m_IdleFence), "Failed to create idle fence");

        auto& DevDesc = m_CoreInterface->GetDeviceDesc(RhiDevice);

        if (!TryLoadFeature(RhiDevice, m_SwapChainInterface, SwapchainFeatures, "SwapChainInterface"))
        {
            Shutdown();
            return false;
        }
        if (!TryLoadFeature(RhiDevice, m_MeshShaderInterface, MeshShaderFeatures, "MeshShaderInterface", DevDesc.isMeshShaderSupported))
        {
            Shutdown();
            return false;
        }
        if (!TryLoadFeature(RhiDevice, m_RayTracingInterface, RayTracingFeatures, "RayTracingInterface", DevDesc.isRayTracingSupported))
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
			.signalFences = &FenceDesc,
			.signalFenceNum = 1
		};

        m_CoreInterface->QueueSubmit(CommandQueue, SubmitDesc);
        m_CoreInterface->Wait(*m_IdleFence, m_FenceValue);
    }
} // namespace Ame::Rhi