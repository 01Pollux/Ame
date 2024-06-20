#include <Rhi/Device/WindowManager.hpp>

#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

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

    WindowManager::WindowManager(
        DeviceImpl&             rhiDevice,
        const DeviceCreateDesc& deviceCreateDesc) :
        m_Device(rhiDevice),
        m_Window(deviceCreateDesc.Window->Window),
        m_SwapChainFormat(ToSwapchainFormat(deviceCreateDesc.Window->Format)),
        m_OnWindowSizeChanged(m_Window.OnWindowSizeChanged([this](const auto&)
                                                           { m_DirtySwapChain = true; })),
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
            auto& nriUtils     = m_Device.GetNRI();
            auto& nriSwapchain = *nriUtils.GetSwapChainInterface();

            nriSwapchain.DestroySwapChain(*m_SwapChain);
        }
    }

    //

    void WindowManager::NewFrame()
    {
        auto& nriUtils     = m_Device.GetNRI();
        auto& nriSwapchain = *nriUtils.GetSwapChainInterface();

        m_BackBufferIndex = nriSwapchain.AcquireNextSwapChainTexture(*m_SwapChain);
    }

    void WindowManager::Present()
    {
        auto& nriUtils     = m_Device.GetNRI();
        auto& nriSwapchain = *nriUtils.GetSwapChainInterface();

        nriSwapchain.QueuePresent(*m_SwapChain);
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

    Windowing::Window* WindowManager::GetWindow()
    {
        return &m_Window;
    }

    nri::SwapChain& WindowManager::GetSwapChain()
    {
        return *m_SwapChain;
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
        auto& nriUtils      = m_Device.GetNRI();
        auto& nriSwapchain  = *nriUtils.GetSwapChainInterface();
        auto& graphicsQueue = m_Device.GetQueue();

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
            .commandQueue         = &graphicsQueue,
            .width                = static_cast<nri::Dim_t>(windowSize.x()),
            .height               = static_cast<nri::Dim_t>(windowSize.y()),
            .textureNum           = static_cast<uint8_t>(m_BackBuffers.size()),
            .format               = m_SwapChainFormat,
            .verticalSyncInterval = m_VSyncEnabled
        };

        ThrowIfFailed(nriSwapchain.CreateSwapChain(m_Device.GetDevice(), swapChainDesc, m_SwapChain), "Failed to create swapchain");
        CreateSwapChainViews();
    }

    void WindowManager::CreateSwapChainViews()
    {
        auto& nriUtils     = m_Device.GetNRI();
        auto& nriSwapchain = *nriUtils.GetSwapChainInterface();
        auto& nriUtilsCore = *nriUtils.GetCoreInterface();

        uint32_t count       = 0;
        auto     backBuffers = nriSwapchain.GetSwapChainTextures(*m_SwapChain, count);
        m_BackBufferFormat   = nriUtilsCore.GetTextureDesc(*backBuffers[0]).format;

        nri::Texture2DViewDesc viewDesc{
            .viewType = nri::Texture2DViewType::COLOR_ATTACHMENT,
            .format   = m_BackBufferFormat
        };

        for (uint32_t i = 0; i < count; i++)
        {
            viewDesc.texture          = backBuffers[i];
            m_BackBuffers[i].Resource = Texture(Texture::Extern{}, m_Device, backBuffers[i]);

            nri::Descriptor* view = nullptr;
            ThrowIfFailed(nriUtilsCore.CreateTexture2DView(viewDesc, view), "Failed to create texture view of the swapchain texture");
            m_BackBuffers[i].View = RenderTargetResourceView(ResourceView::Extern{}, m_Device, view);
        }
    }

    void WindowManager::ReleaseBackBuffers()
    {
        auto& nriUtils     = m_Device.GetNRI();
        auto& nriUtilsCore = *nriUtils.GetCoreInterface();

        for (auto& backBuffer : m_BackBuffers)
        {
            if (auto descriptor = backBuffer.View.Unwrap())
            {
                nriUtilsCore.DestroyDescriptor(*descriptor);
                backBuffer.View = {};
            }
        }
    }
} // namespace Ame::Rhi