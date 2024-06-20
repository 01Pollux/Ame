#include <Rhi/Device/WindowManager.hpp>
#include <Rhi/Device/Wrapper/DeviceWrapper.hpp>

#include <Rhi/Device/CreateDesc.hpp>

#include <Rhi/Nri/Nri.hpp>
#include <Rhi/NriError.hpp>

#if defined AME_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined AME_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#else
#error "Unknown platform for GLFW"
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Ame::Rhi
{
    static nri::SwapChainFormat ToSwapchainFormat(
        SwapChainFormat format)
    {
        switch (format)
        {
        case SwapChainFormat::BT709_G10_16BIT:
            return nri::SwapChainFormat::BT709_G10_16BIT;
        case SwapChainFormat::BT709_G22_8BIT:
            return nri::SwapChainFormat::BT709_G22_8BIT;
        case SwapChainFormat::BT709_G22_10BIT:
            return nri::SwapChainFormat::BT709_G22_10BIT;
        case SwapChainFormat::BT2020_G2084_10BIT:
            return nri::SwapChainFormat::BT2020_G2084_10BIT;
        default:
            std::unreachable();
            break;
        }
    }

    static SwapChainFormat FromSwapchainFormat(
        nri::SwapChainFormat format)
    {
        switch (format)
        {
        case nri::SwapChainFormat::BT709_G10_16BIT:
            return SwapChainFormat::BT709_G10_16BIT;
        case nri::SwapChainFormat::BT709_G22_8BIT:
            return SwapChainFormat::BT709_G22_8BIT;
        case nri::SwapChainFormat::BT709_G22_10BIT:
            return SwapChainFormat::BT709_G22_10BIT;
        case nri::SwapChainFormat::BT2020_G2084_10BIT:
            return SwapChainFormat::BT2020_G2084_10BIT;
        default:
            std::unreachable();
            break;
        }
    }

    //

    WindowManager::WindowManager(
        IDeviceWrapper&         deviceWrapper,
        const DeviceCreateDesc& deviceCreateDesc) :
        m_Window(deviceCreateDesc.Window->Window),
        m_DeviceWrapper(deviceWrapper),
        m_OnWindowSizeChanged(m_Window.OnWindowSizeChanged([this](const auto&)
                                                           { m_DirtySwapChain = true; })),
        m_SwapChainFormat(ToSwapchainFormat(deviceCreateDesc.Window->Format)),
        m_BackBuffers(deviceCreateDesc.Window->BackbufferCount),
        m_VSyncEnabled(deviceCreateDesc.EnableVSync)
    {
        RecreateSwapchain();
    }

    WindowManager::~WindowManager()
    {
        ReleaseBackBuffers();
        if (m_SwapChain)
        {
            auto& nri          = m_DeviceWrapper.get().GetNri();
            auto& nriSwapchain = *nri.GetSwapChainInterface();

            nriSwapchain.DestroySwapChain(*m_SwapChain);
            m_SwapChain = nullptr;
        }
    }

    //

    void WindowManager::NewFrame()
    {
        auto& nri          = m_DeviceWrapper.get().GetNri();
        auto& nriSwapchain = *nri.GetSwapChainInterface();

        m_BackBufferIndex = nriSwapchain.AcquireNextSwapChainTexture(*m_SwapChain);
    }

    void WindowManager::Present()
    {
        auto& nri          = m_DeviceWrapper.get().GetNri();
        auto& nriSwapchain = *nri.GetSwapChainInterface();

        nriSwapchain.QueuePresent(*m_SwapChain);
    }

    //

    Windowing::Window* WindowManager::GetWindow()
    {
        return &m_Window;
    }

    nri::SwapChain& WindowManager::GetSwapChain()
    {
        return *m_SwapChain;
    }

    SwapChainFormat WindowManager::GetSwapchainFormat() const
    {
        return FromSwapchainFormat(m_SwapChainFormat);
    }

    ResourceFormat WindowManager::GetBackbufferFormat() const
    {
        return m_BackBufferFormat;
    }

    //

    uint8_t WindowManager::GetBackbufferCount() const
    {
        return static_cast<uint8_t>(m_BackBuffers.size());
    }

    uint8_t WindowManager::GetBackbufferIndex() const
    {
        return m_BackBufferIndex;
    }

    const Backbuffer& WindowManager::GetBackbuffer(
        uint8_t index) const
    {
        return m_BackBuffers[index];
    }

    //

    bool WindowManager::IsVSyncEnabled() const noexcept
    {
        return m_VSyncEnabled;
    }

    void WindowManager::SetVSyncEnabled(
        bool State)
    {
        if (m_VSyncEnabled != State)
        {
            m_VSyncEnabled   = State;
            m_DirtySwapChain = true;
        }
    }

    //

    bool WindowManager::IsDirty() const noexcept
    {
        return m_DirtySwapChain;
    }

    void WindowManager::RecreateSwapchain()
    {
        auto& nri          = m_DeviceWrapper.get().GetNri();
        auto& nriSwapchain = *nri.GetSwapChainInterface();
        auto& device       = m_DeviceWrapper.get().GetNriDevice();
        auto& presentQueue = m_DeviceWrapper.get().GetGraphicsQueue();

        m_DirtySwapChain = false;
        ReleaseBackBuffers();

        if (m_SwapChain)
        {
            nriSwapchain.DestroySwapChain(*m_SwapChain);
            m_SwapChain = nullptr;
        }

        auto windowSize   = m_Window.GetSize();
        auto windowHandle = m_Window.GetHandle();

        nri::Window nriWindow
        {
#if AME_PLATFORM_WINDOWS
            .windows = { glfwGetWin32Window(windowHandle) }
#elif AME_PLATFORM_LINUX
            .x11 = {
                .dpy    = glfwGetX11Display(),
                .window = glfwGetX11Window(windowHandle)
            }
#endif
        };

        nri::SwapChainDesc swapChainDesc{
            .window               = nriWindow,
            .commandQueue         = &presentQueue,
            .width                = static_cast<nri::Dim_t>(windowSize.x()),
            .height               = static_cast<nri::Dim_t>(windowSize.y()),
            .textureNum           = static_cast<uint8_t>(m_BackBuffers.size()),
            .format               = m_SwapChainFormat,
            .verticalSyncInterval = m_VSyncEnabled
        };

        ThrowIfFailed(nriSwapchain.CreateSwapChain(device, swapChainDesc, m_SwapChain), "Failed to create swapchain");
        CreateSwapChainViews();
    }

    void WindowManager::CreateSwapChainViews()
    {
        auto& nri          = m_DeviceWrapper.get().GetNri();
        auto& nriCore      = *nri.GetCoreInterface();
        auto& nriSwapchain = *nri.GetSwapChainInterface();

        uint32_t count       = 0;
        auto     backBuffers = nriSwapchain.GetSwapChainTextures(*m_SwapChain, count);
        m_BackBufferFormat   = nriCore.GetTextureDesc(*backBuffers[0]).format;

        nri::Texture2DViewDesc viewDesc{
            .viewType = nri::Texture2DViewType::COLOR_ATTACHMENT,
            .format   = m_BackBufferFormat
        };

        for (uint32_t i = 0; i < count; i++)
        {
            viewDesc.texture          = backBuffers[i];
            m_BackBuffers[i].Resource = Texture(nriCore, backBuffers[i]);

            nri::Descriptor* view = nullptr;
            ThrowIfFailed(nriCore.CreateTexture2DView(viewDesc, view), "Failed to create texture view of the swapchain texture");
            m_BackBuffers[i].View = ResourceView(nriCore, view);
        }
    }

    void WindowManager::ReleaseBackBuffers()
    {
        auto& nri     = m_DeviceWrapper.get().GetNri();
        auto& nriCore = *nri.GetCoreInterface();

        for (auto& backBuffer : m_BackBuffers)
        {
            if (auto descriptor = backBuffer.View.Unwrap())
            {
                nriCore.DestroyDescriptor(*descriptor);
                backBuffer = {};
            }
        }
    }
} // namespace Ame::Rhi