#pragma once

#include <Rhi/Core.hpp>
#include "../Nri/Nri.hpp"

namespace Ame::Rhi
{
    struct DeviceCreationDesc;

    class NRIBridge final
    {
    public:
        /// <summary>
        /// Attempts to initialize the bridge.
        /// Fails if the swap chain, mesh shader, or ray tracing extensions are not supported.
        /// </summary>
        bool Initialize(
            nri::Device&      RhiDevice,
            DeviceFeatureType SwapchainFeatures,
            DeviceFeatureType MeshShaderFeatures,
            DeviceFeatureType RayTracingFeatures);

        /// <summary>
        /// Releases all resources used by the bridge.
        /// </summary>
        void Shutdown();

    public:
        /// <summary>
        /// Helper function to wait for the GPU to idle.
        /// </summary>
        void WaitIdle(
            nri::CommandQueue& CommandQueue);

    public:
        [[nodiscard]] nri::CoreInterface* GetCoreInterface() const
        {
            return m_CoreInterface.get();
        }
        [[nodiscard]] nri::SwapChainInterface* GetSwapChainInterface() const
        {
            return m_SwapChainInterface.get();
        }
        [[nodiscard]] nri::MeshShaderInterface* GetMeshShaderInterface() const
        {
            return m_MeshShaderInterface.get();
        }
        [[nodiscard]] nri::RayTracingInterface* GetRayTracingInterface() const
        {
            return m_RayTracingInterface.get();
        }

    private:
        UPtr<nri::CoreInterface>       m_CoreInterface;
        UPtr<nri::SwapChainInterface>  m_SwapChainInterface;
        UPtr<nri::MeshShaderInterface> m_MeshShaderInterface;
        UPtr<nri::RayTracingInterface> m_RayTracingInterface;

        nri::Fence* m_IdleFence  = nullptr;
        uint64_t m_FenceValue = 0;
    };
} // namespace Ame::Rhi