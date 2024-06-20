#ifdef AME_PLATFORM_WINDOWS
#include <stdexcept>

#include <Rhi/Device/Wrapper/D3D12/Device.hpp>
#include <Rhi/Device/Wrapper/D3D12/MemoryAllocator.hpp>

#include <Rhi/Nri/Allocator.hpp>
#include <Rhi/Nri/Log.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi::D3D12
{
    D3D12DeviceWrapper::D3D12DeviceWrapper(
        const DeviceCreateDesc& createDesc)
    {
        EnableValidationIfNeeded(createDesc);
        CreateD3D12Device(createDesc);
        SuppressWarningsIfNeeded(m_D3D12Device.Get(), createDesc);
        CreateNRIDevice(createDesc);
    }

    bool D3D12DeviceWrapper::Initialize(
        const DeviceCreateDesc& createDesc)
    {
        if (!IDeviceWrapper::Initialize(createDesc))
        {
            return false;
        }

        if (nri::nriGetInterface(*m_Device, NRI_INTERFACE(nri::WrapperD3D12Interface), &m_NRID3D12) != nri::Result::SUCCESS)
        {
            return false;
        }

        D3D12DeviceMemoryAllocatorDesc memoryAllocatorDesc{
            .NriDevice  = *m_Device,
            .NriCore    = *m_NRI.GetCoreInterface(),
            .NriD3D12   = m_NRID3D12,
            .MemoryDesc = createDesc.MemoryDesc,
            .Device     = m_D3D12Device.Get(),
            .Adapter    = m_D3D12Adapter.Get()
        };
        m_MemoryAllocator = std::make_unique<D3D12DeviceMemoryAllocator>(memoryAllocatorDesc);
        return true;
    }

    //

    void D3D12DeviceWrapper::CreateD3D12Device(
        const DeviceCreateDesc& createDesc)
    {
        ThrowIfHrFailed(
            CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory)),
            "Failed to create DXGI factory.");
        m_D3D12Adapter = GetBestAdapter(m_DxgiFactory.Get(), createDesc.Adapter ? &createDesc.Adapter : nullptr);
        if (!m_D3D12Adapter)
        {
            throw std::runtime_error("Failed to find a suitable adapter.");
        }

        ThrowIfHrFailed(
            D3D12CreateDevice(m_D3D12Adapter.Get(), c_MinFeatureLevel, IID_PPV_ARGS(&m_D3D12Device)),
            "Failed to create D3D12 device.");
    }

    void D3D12DeviceWrapper::CreateNRIDevice(
        const DeviceCreateDesc& createDesc)
    {
        nri::DeviceCreationD3D12Desc nriDeviceDesc{
            .d3d12Device = m_D3D12Device.Get(),
            .callbackInterface{
                .MessageCallback = NriLogCallbackInterface::MessageCallback,
                .AbortExecution  = NriLogCallbackInterface::AbortExecution },
            .memoryAllocatorInterface{
                .Allocate   = NriAllocatorCallbackInterface::Allocate,
                .Reallocate = NriAllocatorCallbackInterface::Reallocate,
                .Free       = NriAllocatorCallbackInterface::Free },
            .enableD3D12DrawParametersEmulation = c_EnableDrawParametersEmulation,
            .enableNRIValidation                = createDesc.EnableApiValidationLayer
        };

        if (nri::nriCreateDeviceFromD3D12Device(nriDeviceDesc, m_Device) != nri::Result::SUCCESS)
        {
            throw std::runtime_error("Failed to create NRI device.");
        }
    }

    //

    void D3D12DeviceWrapper::SuppressWarningsIfNeeded(
        ID3D12Device*           d3d12Device,
        const DeviceCreateDesc& createDesc)
    {
#ifndef AME_DIST
        if (createDesc.EnableApiValidationLayer) [[likely]]
        {
            ID3D12InfoQueue* d3d12InfoQueue = nullptr;
            d3d12Device->QueryInterface(&d3d12InfoQueue);

            if (d3d12InfoQueue)
            {
                D3D12_MESSAGE_ID disableMessageIDs[] = {
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
                };

                D3D12_INFO_QUEUE_FILTER filter{
                    .DenyList{
                        .NumIDs  = 1,
                        .pIDList = disableMessageIDs }
                };

                d3d12InfoQueue->AddStorageFilterEntries(&filter);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
                d3d12InfoQueue->Release();
            }
        }
#endif
    }

    void D3D12DeviceWrapper::EnableValidationIfNeeded(
        const DeviceCreateDesc& createDesc)
    {
#ifndef AME_DIST
        if (createDesc.EnableApiValidationLayer) [[likely]]
        {
            ID3D12Debug* d3d12Debug = nullptr;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug))))
            {
                d3d12Debug->EnableDebugLayer();
                d3d12Debug->Release();
            }
        }
#endif
    }

    //

    auto D3D12DeviceWrapper::GetBestAdapter(
        IDXGIFactory1*          dxgiFactory,
        const nri::AdapterDesc* adapterDesc) -> ComPtr<IDXGIAdapter>
    {
        ComPtr<IDXGIAdapter1> curAdapter;
        ComPtr<IDXGIAdapter1> firstBestAdapter;

        DXGI_ADAPTER_DESC1 curAdapterDesc;

        for (uint32_t i = 0; SUCCEEDED(dxgiFactory->EnumAdapters1(i, curAdapter.ReleaseAndGetAddressOf())); i++)
        {
            // Get the adapter description.
            if (FAILED(curAdapter->GetDesc1(&curAdapterDesc)))
            {
                continue;
            }

            // Skip software adapters.
            if (curAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(
                    curAdapter.Get(),
                    c_MinFeatureLevel,
                    __uuidof(ID3D12Device),
                    nullptr)))
            {
                if (adapterDesc)
                {
                    LUID luid{
                        .LowPart  = static_cast<uint32_t>(adapterDesc->luid),
                        .HighPart = static_cast<long>(adapterDesc->luid >> 32)
                    };
                    if ((curAdapterDesc.AdapterLuid.LowPart == luid.LowPart) &&
                        (curAdapterDesc.AdapterLuid.HighPart == luid.HighPart))
                    {
                        firstBestAdapter = curAdapter;
                        break;
                    }
                }
                else
                {
                    firstBestAdapter = curAdapter;
                    break;
                }
            }
        }

        return firstBestAdapter;
    }
} // namespace Ame::Rhi::D3D12
#endif
