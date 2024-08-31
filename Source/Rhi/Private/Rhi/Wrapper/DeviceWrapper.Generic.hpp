#pragma once

#include <Core/Enum.hpp>
#include <Window/Window.hpp>

#include <Rhi/Wrapper/DeviceWrapper.hpp>
#include <Window/DesktopWindow.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Window
{
    class IDesktopWindow;
} // namespace Ame::Window

namespace Ame::Rhi
{
    template<typename Ty> [[nodiscard]] static void GetOrDefault(Ty& finalValue, const Ty& value) noexcept
    {
        if (static_cast<bool>(value))
        {
            finalValue = value;
        }
    }

    template<typename Ty> [[nodiscard]] static Ptr<Ty> GetWindowForSurface(const RenderSurfaceDesc& surface)
    {
        Ptr<Window::IDesktopWindow> desktopWindow;
        surface.Window->QueryInterface(Window::IID_DesktopWindow, desktopWindow.DblPtr<IObject>());
        return desktopWindow;
    }

    template<typename Ty>
    concept DeviceCreateTraits = requires {
        typename Ty::create_struct_type;
        typename Ty::diligent_create_info;
        typename Ty::diligent_factory_type;

        {
            Ty::GetName()
        } -> std::same_as<const char*>;

        {
            Ty::GetCreateInfo(std::declval<DeviceCreateDesc>(), std::declval<typename Ty::create_struct_type>())
        } -> std::same_as<typename Ty::diligent_create_info>;

        {
            Ty::LoadFactory()
        } -> std::same_as<typename Ty::diligent_factory_type*>;

        {
            Ty::CreateDeviceAndContext(std::declval<typename Ty::diligent_factory_type*>(),
                                       std::declval<typename Ty::diligent_create_info>(),
                                       std::declval<Dg::IRenderDevice**>(), std::declval<Dg::IDeviceContext**>())
        };

        {
            Ty::CreateSwapchain(std::declval<typename Ty::diligent_factory_type*>(), std::declval<Dg::IRenderDevice*>(),
                                std::declval<Dg::IDeviceContext*>(), std::declval<Dg::SwapChainDesc>(),
                                std::declval<Dg::FullScreenModeDesc>(), std::declval<const Dg::NativeWindow&>(),
                                std::declval<Dg::ISwapChain**>())
        };
    };

    //

    /// <summary>
    /// Find the best adapter for the Diligent engine
    /// </summary>
    [[nodiscard]] uint32_t FindDiligentBestAdapter(Dg::IEngineFactory*     engineFactory,
                                                   const DeviceCreateDesc& createDesc,
                                                   const Dg::Version&      version = Dg::Version{ 0, 0 });

    /// <summary>
    /// Parse the device create description.
    /// </summary>
    void ParseDiligentEngineCreateDesc(const DeviceCreateDesc& createDesc, Dg::EngineCreateInfo& createInfo);

    //

    /// <summary>
    /// Create a swapchain description.
    /// </summary>
    Dg::SwapChainDesc CreateDiligentSwapChainDesc(Window::IDesktopWindow* desktopWindow,
                                                  const SwapchainDesc&    swapchainDesc);

    /// <summary>
    /// Create a fullscreen mode description.
    /// </summary>
    Dg::FullScreenModeDesc CreateDiligentFullscreenDesc(Window::IDesktopWindow*   desktopWindow,
                                                        const FullscreenModeDesc& fullscreenDesc);

    /// <summary>
    /// Convert a window to a Diligent native window struct
    /// </summary>
    [[nodiscard]] Dg::NativeWindow GetDiligentNativeWindow(Window::IDesktopWindow* desktopWindow, bool isGL = false);

    /// <summary>
    /// Set the debug callback for the Diligent engine
    /// </summary>
    void SetMessageCallback(Dg::IEngineFactory* factory);

    //

    template<DeviceCreateTraits Ty> struct GenericDeviceCreator
    {
        using traits_type          = Ty;
        using create_struct_type   = typename traits_type::create_struct_type;
        using diligent_create_info = typename traits_type::diligent_create_info;

        [[nodiscard]] static Opt<DeviceWrapper> Create(const DeviceCreateDesc&   createDesc,
                                                       const create_struct_type& createDescDev)
        {
            using namespace EnumBitOperators;

            Opt<DeviceWrapper> deviceWrapper;

            Ptr<Dg::IEngineFactory> engineFactory;
            Ptr<Dg::IRenderDevice>  renderDevice;
            Ptr<Dg::IDeviceContext> deviceContext;
            Ptr<Dg::ISwapChain>     swapchain;

            UniquePtr<WindowWrapper> windowWrapper;

            auto createInfo = traits_type::GetCreateInfo(createDesc, createDescDev);
            ParseDiligentEngineCreateDesc(createDesc, createInfo);

            auto factoryDev = traits_type::LoadFactory();
            if (factoryDev)
            {
                SetMessageCallback(factoryDev);
                // check if createDescDev has GrapihcsAPIVersion member
                if constexpr (requires { createDescDev.GraphicsAPIVersion; })
                {
                    createInfo.AdapterId =
                        FindDiligentBestAdapter(factoryDev, createDesc, createDescDev.GraphicsAPIVersion);
                }
                else
                {
                    createInfo.AdapterId = FindDiligentBestAdapter(factoryDev, createDesc);
                }
                traits_type::CreateDeviceAndContext(factoryDev, createInfo, &renderDevice, &deviceContext);
            }

            if (!(factoryDev && renderDevice && deviceContext))
            {
                Log::Rhi().Warning("Failed to load {} graphics engine", traits_type::GetName());
                return deviceWrapper;
            }
            factoryDev->QueryInterface(Dg::IID_EngineFactory, engineFactory.RawDblPtr<Dg::IObject>());

            if (createDesc.Surface)
            {
                auto& surfaceDesc = *createDesc.Surface;
                if (!surfaceDesc.Window)
                {
                    Log::Rhi().Warning(
                        "Failed to create swapchain for {} graphics engine: Window is null", traits_type::GetName());
                    return deviceWrapper;
                }

                auto desktopWindow  = GetWindowForSurface<Window::IDesktopWindow>(surfaceDesc);
                auto swapchainDesc  = CreateDiligentSwapChainDesc(desktopWindow, surfaceDesc.Swapchain);
                auto fullscreenDesc = CreateDiligentFullscreenDesc(desktopWindow, surfaceDesc.FullscreenMode);

                traits_type::CreateSwapchain(factoryDev, renderDevice, deviceContext, swapchainDesc, fullscreenDesc,
                                             GetDiligentNativeWindow(desktopWindow), &swapchain);
                if (!swapchain)
                {
                    Log::Rhi().Warning("Failed to create swapchain for {} graphics engine", traits_type::GetName());
                    return deviceWrapper;
                }

                windowWrapper = std::make_unique<WindowWrapper>(Ptr(surfaceDesc.Window), std::move(swapchain));
            }

            deviceWrapper.emplace(
                std::move(engineFactory), std::move(renderDevice), std::move(deviceContext), std::move(windowWrapper));
            return deviceWrapper;
        }
    };
} // namespace Ame::Rhi