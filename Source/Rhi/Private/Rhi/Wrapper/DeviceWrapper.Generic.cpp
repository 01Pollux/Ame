#include <Rhi/Wrapper/DeviceWrapper.Generic.hpp>

#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(BOOST_OS_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Ame::Rhi
{
    uint32_t FindDiligentBestAdapter(Dg::IEngineFactory* engineFactory, const DeviceCreateDesc& createDesc,
                                     const Dg::Version& version)
    {
        Dg::Uint32 numAdapters = 0;
        engineFactory->EnumerateAdapters(version, numAdapters, nullptr);
        std::vector<GraphicsAdapterInfo> adapters(numAdapters);
        engineFactory->EnumerateAdapters(version, numAdapters, adapters.data());

        Opt<uint32_t> bestAdapter;
        if (adapters.empty())
        {
            AME_LOG_WARNING("No adapters found");
            return Dg::DEFAULT_ADAPTER_ID;
        }
        if (createDesc.AdapterCallback)
        {
            bestAdapter = createDesc.AdapterCallback(adapters);
        }

        return bestAdapter
            .or_else(
                [&]()
                {
                    Opt<uint32_t> defaultAdapter;
                    for (uint32_t i = 0; i < numAdapters; ++i)
                    {
                        const auto& adapter = adapters[i];
                        if (adapter.Type == Diligent::ADAPTER_TYPE_INTEGRATED ||
                            adapter.Type == Diligent::ADAPTER_TYPE_DISCRETE)
                        {
                            defaultAdapter = i;
                            // Always prefer discrete gpu
                            if (adapter.Type == Diligent::ADAPTER_TYPE_DISCRETE)
                            {
                                break;
                            }
                        }
                    }
                    return defaultAdapter;
                })
            .value_or(Dg::DEFAULT_ADAPTER_ID);
    }

    void ParseDiligentEngineCreateDesc(const DeviceCreateDesc& createDesc, Dg::EngineCreateInfo& createInfo)
    {
        using namespace EnumBitOperators;

        createInfo.Features = createDesc.Features;

        createInfo.EnableValidation =
            (createDesc.ValidationLayer & DeviceValidationType::DebugLayer) == DeviceValidationType::DebugLayer;
        if ((createDesc.ValidationLayer & DeviceValidationType::CheckShaderBufferSize) ==
            DeviceValidationType::DebugLayer)
        {
            createInfo.ValidationFlags |= Dg::VALIDATION_FLAGS::VALIDATION_FLAG_CHECK_SHADER_BUFFER_SIZE;
        }

        createInfo.pRawMemAllocator = &ObjectMemoryAllocator::Instance();
    }

    //

    Dg::SwapChainDesc CreateDiligentSwapChainDesc(Window::IWindow* window, const SwapchainDesc& swapchainDesc)
    {
        auto              size = window->GetSize();
        Dg::SwapChainDesc dgSwapchainDesc;

        dgSwapchainDesc.Width             = static_cast<Dg::Uint32>(size.x());
        dgSwapchainDesc.Height            = static_cast<Dg::Uint32>(size.y());
        dgSwapchainDesc.ColorBufferFormat = swapchainDesc.ColorFormat;
        dgSwapchainDesc.DepthBufferFormat = swapchainDesc.DepthFormat;
        dgSwapchainDesc.Usage             = swapchainDesc.Usage;
        dgSwapchainDesc.PreTransform      = swapchainDesc.PreTransform;
        GetOrDefault(dgSwapchainDesc.BufferCount, swapchainDesc.BufferCount);
        dgSwapchainDesc.DefaultDepthValue   = swapchainDesc.DefaultDepthValue;
        dgSwapchainDesc.DefaultStencilValue = swapchainDesc.DefaultStencilValue;
        dgSwapchainDesc.IsPrimary           = swapchainDesc.IsPrimary;

        return dgSwapchainDesc;
    }

    Dg::FullScreenModeDesc CreateDiligentFullscreenDesc(Window::IWindow*, const FullscreenModeDesc& fullscreenDesc)
    {
        return { .Fullscreen             = fullscreenDesc.Fullscreen,
                 .RefreshRateNumerator   = fullscreenDesc.RefreshRate.Numerator,
                 .RefreshRateDenominator = fullscreenDesc.RefreshRate.Denominator,
                 .Scaling                = fullscreenDesc.Scaling,
                 .ScanlineOrder          = fullscreenDesc.ScanlineOrder };
    }

    Dg::NativeWindow GetDiligentNativeWindow(Window::IWindow* window, [[maybe_unused]] bool isGL)
    {
        auto glfwWindow = window->GetGlfwHandle();

#if PLATFORM_WIN32
        Dg::Win32NativeWindow handle{ glfwGetWin32Window(glfwWindow) };
#endif

#if PLATFORM_LINUX
        Dg::LinuxNativeWindow handle;
        handle.WindowId = glfwGetX11Window(glfwWindow);
        handle.pDisplay = glfwGetX11Display();
        if (isGL)
            glfwMakeContextCurrent(glfwWindow);
#endif

        return handle;
    }

    //

    void DILIGENT_CALL_TYPE OnDiligentMessageCallback(Dg::DEBUG_MESSAGE_SEVERITY severity, const Dg::Char* message,
                                                      [[maybe_unused]] const Dg::Char* function,
                                                      [[maybe_unused]] const Dg::Char* file, [[maybe_unused]] int line)
    {
        Log::LogLevel level = Log::LogLevel::Disabled;
        switch (severity)
        {
        case Dg::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_INFO:
            level = Log::LogLevel::Info;
            break;
        case Dg::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_WARNING:
            level = Log::LogLevel::Warning;
            break;
        case Dg::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_ERROR:
            level = Log::LogLevel::Error;
            break;
        case Dg::DEBUG_MESSAGE_SEVERITY::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
            level = Log::LogLevel::Fatal;
            break;
        }
        if (Log::s_Logger && Log::s_Logger->CanLog(level))
        {
#ifdef AME_DIST
            Log::s_Logger->WriteMessage({ level, message });
#else
            StringView logFormat;
            uint32_t   code = (function ? 1 : 0) | (file ? 2 : 0);

            function = function ? function : "";
            file     = file ? file : "";

            switch (code)
            {
            case 0:
                Log::s_Logger->WriteMessage({ level, message });
                break;
            case 1:
            case 2:
                Log::s_Logger->WriteMessage({ level, std::format("{} (L{}): {}", file, function, line, message) });
                break;
            case 3:
                Log::s_Logger->WriteMessage({ level, std::format("{}/{} (L{}): {}", file, function, line, message) });
                break;
            default:
                std::unreachable();
            }
#endif
        }
    }

    void SetMessageCallback(Dg::IEngineFactory* factory)
    {
        Dg::SetDebugMessageCallback(&OnDiligentMessageCallback);
        factory->SetMessageCallback(&OnDiligentMessageCallback);
    }
} // namespace Ame::Rhi