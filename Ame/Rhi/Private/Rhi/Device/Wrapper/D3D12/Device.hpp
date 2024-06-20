#pragma once

#ifdef AME_PLATFORM_WINDOWS

#include <Rhi/Device/Wrapper/D3D12/Core.hpp>
#include <Rhi/Device/Wrapper/DeviceWrapper.hpp>

namespace Ame::Rhi::D3D12
{
    class D3D12DeviceWrapper : public IDeviceWrapper
    {
        template<typename Ty>
        using ComPtr = Microsoft::WRL::ComPtr<Ty>;

        static inline D3D_FEATURE_LEVEL c_MinFeatureLevel = D3D_FEATURE_LEVEL_12_0;

    public:
        D3D12DeviceWrapper(
            const DeviceCreateDesc& createDesc);

    protected:
        bool Initialize(
            const DeviceCreateDesc& createDesc) override;

    private:
        /// <summary>
        /// Create the D3D12 device.
        /// </summary>
        void CreateD3D12Device(
            const DeviceCreateDesc& createDesc);

        /// <summary>
        /// Create the NRI device.
        /// </summary>
        void CreateNRIDevice(
            const DeviceCreateDesc& createDesc);

    private:
        /// <summary>
        /// Suppresses warnings if needed.
        /// This is used for debugging purposes.
        /// </summary>
        static void SuppressWarningsIfNeeded(
            ID3D12Device*           d3d12Device,
            const DeviceCreateDesc& createDesc);

        /// <summary>
        /// Enables validation if needed.
        /// </summary>
        static void EnableValidationIfNeeded(
            const DeviceCreateDesc& createDesc);

        /// <summary>
        /// Fetches the best adapter if required.
        /// </summary>
        static ComPtr<IDXGIAdapter> GetBestAdapter(
            IDXGIFactory1*          dxgiFactory,
            const nri::AdapterDesc* adapterDesc);

    private:
        ComPtr<IDXGIFactory1> m_DxgiFactory;
        ComPtr<IDXGIAdapter>  m_D3D12Adapter;
        ComPtr<ID3D12Device>  m_D3D12Device;

        nri::WrapperD3D12Interface m_NRID3D12;
    };
} // namespace Ame::Rhi::D3D12
#endif
