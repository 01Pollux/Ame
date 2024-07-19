#include <Rhi/Wrapper/DeviceWrapper.Generic.hpp>

#ifdef D3D11_SUPPORTED
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h>
#endif

namespace Ame::Rhi
{
#ifdef D3D11_SUPPORTED
    static void EnableFlagsOnCreateInfo(
        Dg::EngineD3D11CreateInfo& createInfo,
        Dg::D3D11_VALIDATION_FLAGS flags,
        DeviceValidationType       validationFlags,
        DeviceValidationType       validationType)
    {
        using namespace EnumBitOperators;
        if ((validationFlags & validationType) == validationType)
        {
            createInfo.D3D11ValidationFlags |= flags;
        }
    }

    struct DeviceCreateTraitsD3D11
    {
        using create_struct_type    = DeviceCreateDescD3D11;
        using diligent_create_info  = Dg::EngineD3D11CreateInfo;
        using diligent_factory_type = Dg::IEngineFactoryD3D11;

        static const char* GetName()
        {
            return "D3D11";
        }

        static diligent_create_info GetCreateInfo(
            const DeviceCreateDesc&   createDesc,
            const create_struct_type& createDescDev)
        {
            diligent_create_info createInfo;

            createInfo.GraphicsAPIVersion = createDescDev.GraphicsAPIVersion;

            EnableFlagsOnCreateInfo(createInfo, Dg::D3D11_VALIDATION_FLAG_VERIFY_COMMITTED_RESOURCE_RELEVANCE,
                                    createDesc.ValidationLayer, DeviceValidationType::CommittedResourceRelevance);

            return createInfo;
        }

        static diligent_factory_type* LoadFactory()
        {
            auto getEngineFactoryD3D11 = Dg::LoadGraphicsEngineD3D11();
            return getEngineFactoryD3D11();
        }

        static void CreateDeviceAndContext(
            diligent_factory_type* factory,
            diligent_create_info   createInfo,
            Dg::IRenderDevice**    renderDevice,
            Dg::IDeviceContext**   deviceContext)
        {
            factory->CreateDeviceAndContextsD3D11(createInfo, renderDevice, deviceContext);
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
            factory->CreateSwapChainD3D11(renderDevice, deviceContext, swapchainDesc, fullscreenDesc, nativeWindow, swapchain);
        }
    };
#endif

    //

    Opt<DeviceWrapper> DeviceWrapper::CreateImpl(
        const DeviceCreateDesc&      createDesc,
        const DeviceCreateDescD3D11& createDescD3D11)
    {
#ifdef D3D11_SUPPORTED
        return GenericDeviceCreator<DeviceCreateTraitsD3D11>::Create(createDesc, createDescD3D11);
#else
        return std::nullopt;
#endif
    }
} // namespace Ame::Rhi