#include <Rhi/Wrapper/DeviceWrapper.Generic.hpp>

#if defined(GL_SUPPORTED) || defined(GLES_SUPPORTED)
#include <DiligentCore/Platforms/interface/NativeWindow.h>
#include <DiligentCore/Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>
#include <Window/DesktopWindow.hpp>
#endif

namespace Ame::Rhi
{
#if defined(GL_SUPPORTED) || defined(GLES_SUPPORTED)
    struct DeviceCreateTraitsGL
    {
        using create_struct_type    = DeviceCreateDescGL;
        using diligent_create_info  = Dg::EngineGLCreateInfo;
        using diligent_factory_type = Dg::IEngineFactoryOpenGL;

        static const char* GetName()
        {
            return "GL";
        }

        static diligent_create_info GetCreateInfo(const DeviceCreateDesc& createDesc, const create_struct_type&)
        {
            auto& surfaceDesc = *createDesc.Surface;

            diligent_create_info createInfo;

            auto desktopWindow = GetWindowForSurface<Window::IDesktopWindow>(surfaceDesc);
            createInfo.Window  = GetDiligentNativeWindow(desktopWindow, true);

#ifdef PLATFORM_EMSCRIPTEN
            createInfo.WebGLAttribs.Alpha           = false;
            createInfo.WebGLAttribs.PowerPreference = WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
#endif

            return createInfo;
        }

        static diligent_factory_type* LoadFactory()
        {
            auto getEngineFactoryGL = Dg::LoadGraphicsEngineOpenGL();
            return getEngineFactoryGL ? getEngineFactoryGL() : nullptr;
        }
    };
#endif

    //

    Opt<DeviceWrapper> DeviceWrapper::CreateImpl(const DeviceCreateDesc&   createDesc,
                                                 const DeviceCreateDescGL& createDescDev)
    {
        Opt<DeviceWrapper> deviceWrapper;

#if defined(GL_SUPPORTED) || defined(GLES_SUPPORTED)
        using namespace EnumBitOperators;

        Ptr<Dg::IEngineFactory> engineFactory;
        Ptr<Dg::IRenderDevice>  renderDevice;
        Ptr<Dg::IDeviceContext> deviceContext;
        Ptr<Dg::ISwapChain>     swapchain;

        if (!createDesc.Surface || !createDesc.Surface->Window)
        {
            AME_LOG_WARNING(std::format(
                "Failed to create swapchain for {} graphics engine: Surface is null", DeviceCreateTraitsGL::GetName()));
            return deviceWrapper;
        }

        auto& surfaceDesc = *createDesc.Surface;
        auto  createInfo  = DeviceCreateTraitsGL::GetCreateInfo(createDesc, createDescDev);
        ParseDiligentEngineCreateDesc(createDesc, createInfo);

        auto factoryDev = DeviceCreateTraitsGL::LoadFactory();
        if (factoryDev)
        {
            auto desktopWindow  = GetWindowForSurface<Window::IDesktopWindow>(surfaceDesc);
            auto swapchainDesc  = CreateDiligentSwapChainDesc(desktopWindow, surfaceDesc.Swapchain);
            auto fullscreenDesc = CreateDiligentFullscreenDesc(desktopWindow, surfaceDesc.FullscreenMode);

            createInfo.AdapterId = FindDiligentBestAdapter(factoryDev, createDesc);
            factoryDev->CreateDeviceAndSwapChainGL(
                createInfo, &renderDevice, &deviceContext, swapchainDesc, &swapchain);
        }

        if (!(factoryDev && renderDevice && deviceContext && swapchain))
        {
            AME_LOG_WARNING(std::format("Failed to load {} graphics engine", DeviceCreateTraitsGL::GetName()));
            return deviceWrapper;
        }

        factoryDev->QueryInterface(Dg::IID_EngineFactory, engineFactory.RawDblPtr<Dg::IObject>());
        auto windowWrapper = std::make_unique<WindowWrapper>(Ptr(surfaceDesc.Window), std::move(swapchain));

        deviceWrapper.emplace(
            std::move(engineFactory), std::move(renderDevice), std::move(deviceContext), std::move(windowWrapper));
#endif
        return deviceWrapper;
    }
} // namespace Ame::Rhi