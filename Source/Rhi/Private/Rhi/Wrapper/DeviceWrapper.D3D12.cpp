#include <Rhi/Wrapper/DeviceWrapper.Generic.hpp>

#ifdef D3D12_SUPPORTED
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h>
#include <boost/dll/import.hpp>
#endif

namespace Ame::Rhi
{
#ifdef D3D12_SUPPORTED
    static void EnableFlagsOnCreateInfo(
        Dg::EngineD3D12CreateInfo& createInfo,
        Dg::D3D12_VALIDATION_FLAGS flags,
        DeviceValidationType       validationFlags,
        DeviceValidationType       validationType)
    {
        using namespace EnumBitOperators;
        if ((validationFlags & validationType) == validationType)
        {
            createInfo.D3D12ValidationFlags |= flags;
        }
    }

    struct DeviceCreateTraitsD3D12
    {
        using create_struct_type    = DeviceCreateDescD3D12;
        using diligent_create_info  = Dg::EngineD3D12CreateInfo;
        using diligent_factory_type = Dg::IEngineFactoryD3D12;

        static const char* GetName()
        {
            return "D3D12";
        }

        static diligent_create_info GetCreateInfo(
            const DeviceCreateDesc&   createDesc,
            const create_struct_type& createDescDev)
        {
            diligent_create_info createInfo;

            createInfo.GraphicsAPIVersion = createDescDev.GraphicsAPIVersion;

            EnableFlagsOnCreateInfo(createInfo, Dg::D3D12_VALIDATION_FLAG_BREAK_ON_ERROR,
                                    createDesc.ValidationLayer, DeviceValidationType::BreakOnError);
            EnableFlagsOnCreateInfo(createInfo, Dg::D3D12_VALIDATION_FLAG_BREAK_ON_CORRUPTION,
                                    createDesc.ValidationLayer, DeviceValidationType::BreakOnCorruption);
            EnableFlagsOnCreateInfo(createInfo, Dg::D3D12_VALIDATION_FLAG_ENABLE_GPU_BASED_VALIDATION,
                                    createDesc.ValidationLayer, DeviceValidationType::GpuBasedValidation);

            GetOrDefault(createInfo.CPUDescriptorHeapAllocationSize[0], createDescDev.CpuDescriptors.MaxResources);
            GetOrDefault(createInfo.CPUDescriptorHeapAllocationSize[1], createDescDev.CpuDescriptors.MaxSamplers);
            GetOrDefault(createInfo.CPUDescriptorHeapAllocationSize[2], createDescDev.CpuDescriptors.MaxRenderTargets);
            GetOrDefault(createInfo.CPUDescriptorHeapAllocationSize[3], createDescDev.CpuDescriptors.MaxDepthStencilViews);

            GetOrDefault(createInfo.GPUDescriptorHeapSize[0], createDescDev.GpuDescriptors.MaxResources);
            GetOrDefault(createInfo.GPUDescriptorHeapSize[1], createDescDev.GpuDescriptors.MaxSamplers);

            GetOrDefault(createInfo.GPUDescriptorHeapDynamicSize[0], createDescDev.GpuDynamicDescriptors.MaxResources);
            GetOrDefault(createInfo.GPUDescriptorHeapDynamicSize[1], createDescDev.GpuDynamicDescriptors.MaxSamplers);

            GetOrDefault(createInfo.DynamicDescriptorAllocationChunkSize[0], createDescDev.GpuDynamicDescriptorsChunk.MaxResources);
            GetOrDefault(createInfo.DynamicDescriptorAllocationChunkSize[1], createDescDev.GpuDynamicDescriptorsChunk.MaxSamplers);

            GetOrDefault(createInfo.DynamicHeapPageSize, createDescDev.DynamicHeapPageSize);
            GetOrDefault(createInfo.NumDynamicHeapPagesToReserve, createDescDev.NumDynamicHeapPagesToReserve);

            GetOrDefault(createInfo.QueryPoolSizes[1], createDescDev.QueryHeapSizes.Occlusion);
            GetOrDefault(createInfo.QueryPoolSizes[2], createDescDev.QueryHeapSizes.BinaryOcclusion);
            GetOrDefault(createInfo.QueryPoolSizes[3], createDescDev.QueryHeapSizes.Timestamp);
            GetOrDefault(createInfo.QueryPoolSizes[4], createDescDev.QueryHeapSizes.PipelineStatistics);
            GetOrDefault(createInfo.QueryPoolSizes[5], createDescDev.QueryHeapSizes.Duration);

            return createInfo;
        }

        static diligent_factory_type* LoadFactory()
        {
            diligent_factory_type* factory               = nullptr;
            auto                   getEngineFactoryD3D12 = Dg::LoadGraphicsEngineD3D12();
            if (getEngineFactoryD3D12)
            {
                factory = getEngineFactoryD3D12();
                if (factory)
                {
                    factory->LoadD3D12();
                }
            }
            return factory;
        }

        static void CreateDeviceAndContext(
            diligent_factory_type* factory,
            diligent_create_info   createInfo,
            Dg::IRenderDevice**    renderDevice,
            Dg::IDeviceContext**   deviceContext)
        {
            factory->CreateDeviceAndContextsD3D12(createInfo, renderDevice, deviceContext);
        }

        static void CreateSwapchain(
            diligent_factory_type*        factory,
            Dg::IRenderDevice*            renderDevice,
            Dg::IDeviceContext*           deviceContext,
            const Dg::SwapChainDesc&      swapchainDesc,
            const Dg::FullScreenModeDesc& fullscreenDesc,
            const Dg::NativeWindow&       nativeWindow,
            Dg::ISwapChain**              swapchain)
        {
            factory->CreateSwapChainD3D12(renderDevice, deviceContext, swapchainDesc, fullscreenDesc, nativeWindow, swapchain);
        }
    };
#endif

    //

    Opt<DeviceWrapper> DeviceWrapper::CreateImpl(
        const DeviceCreateDesc&      createDesc,
        const DeviceCreateDescD3D12& createDescDev)
    {
#ifdef D3D12_SUPPORTED
        return GenericDeviceCreator<DeviceCreateTraitsD3D12>::Create(createDesc, createDescDev);
#else
        return std::nullopt;
#endif
    }
} // namespace Ame::Rhi