#include "Bridge.hpp"

#include "../Nri/Nri.hpp"
#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Try to load an nri feature interface.
    /// </summary>
    template<typename Ty>
    static bool TryLoadFeature(
        nri::Device&      rhiDevice,
        const char*       interfaceName,
        size_t            interfaceSize,
        UPtr<Ty>&         interfacePtr,
        DeviceFeatureType features,
        bool              supported = true)
    {
        if (supported)
        {
            if (!interfacePtr)
            {
                interfacePtr = std::make_unique<Ty>();
            }

            if (nri::nriGetInterface(rhiDevice, interfaceName, interfaceSize, interfacePtr.get()) != nri::Result::SUCCESS &&
                features == DeviceFeatureType::Required) [[unlikely]]
            {
                supported = false;
            }
        }
        else if (features != DeviceFeatureType::Required)
        {
            supported = true;
        }
        if (!supported)
        {
            Log::Rhi().Error("Device does not support {}", interfaceName);
        }
        return supported;
    }

    //

    bool NRIBridge::Initialize(
        nri::Device&      rhiDevice,
        DeviceFeatureType swapchainFeatures,
        DeviceFeatureType rayTracingFeatures,
        DeviceFeatureType meshShaderFeatures)
    {
        if (!TryLoadFeature(rhiDevice, NRI_INTERFACE(nri::CoreInterface), m_CoreInterface, DeviceFeatureType::Required))
        {
            Shutdown();
            return false;
        }

        ThrowIfFailed(m_CoreInterface->CreateFence(rhiDevice, 0, m_IdleFence), "Failed to create idle fence");

        auto& DevDesc = m_CoreInterface->GetDeviceDesc(rhiDevice);

        bool succeeded = TryLoadFeature(rhiDevice, NRI_INTERFACE(nri::SwapChainInterface), m_SwapChainInterface, swapchainFeatures) &&
                         TryLoadFeature(rhiDevice, NRI_INTERFACE(nri::MeshShaderInterface), m_MeshShaderInterface, meshShaderFeatures, DevDesc.isMeshShaderSupported) &&
                         TryLoadFeature(rhiDevice, NRI_INTERFACE(nri::RayTracingInterface), m_RayTracingInterface, rayTracingFeatures, DevDesc.isRayTracingSupported);

        if (!succeeded)
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
        nri::CommandQueue& commandQueue)
    {
        nri::FenceSubmitDesc fenceDesc{
            .fence = m_IdleFence,
            .value = ++m_FenceValue
        };

        nri::QueueSubmitDesc submitDesc{
            .signalFences   = &fenceDesc,
            .signalFenceNum = 1
        };

        m_CoreInterface->QueueSubmit(commandQueue, submitDesc);
        m_CoreInterface->Wait(*m_IdleFence, m_FenceValue);
    }
} // namespace Ame::Rhi