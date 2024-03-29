#include "WindowManager.hpp"

#include <Rhi/DeviceCreateDesc.hpp>
#include "../NriError.hpp"

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
        SwapchainFormat Format)
    {
        switch (Format)
        {
        case SwapchainFormat::BT709_G10_16BIT:
            return nri::SwapChainFormat::BT709_G10_16BIT;
        case SwapchainFormat::BT709_G22_8BIT:
            return nri::SwapChainFormat::BT709_G22_8BIT;
        case SwapchainFormat::BT709_G22_10BIT:
            return nri::SwapChainFormat::BT709_G22_10BIT;
        case SwapchainFormat::BT2020_G2084_10BIT:
            return nri::SwapChainFormat::BT2020_G2084_10BIT;
        default:
            std::unreachable();
            break;
        }
    }

    WindowManager::WindowManager(
        NRIBridge&              NriBridge,
        nri::Device&            Device,
        nri::CommandQueue&      PresentQueue,
        const DeviceCreateDesc& Desc) :
        m_NriBridge(NriBridge),
        m_Window(Desc.Window->Window),
        m_SwapChainFormat(ToSwapchainFormat(Desc.Window->SwapChainFormat)),
        m_BackBuffers(Desc.Window->BackbufferCount),
        m_VSyncEnabled(Desc.EnableVSync)
    {
        RecreateSwapchain(Device, PresentQueue);
        m_Window.OnWindowSizeChanged().Listen(
            [this](const Math::Size2I&)
            {
                m_DirtySwapChain = true;
            });
    }

    WindowManager::~WindowManager()
    {
        ReleaseBackBuffers();
        if (m_SwapChain)
        {
            auto& SwapchainInterface = *m_NriBridge.GetSwapChainInterface();
            SwapchainInterface.DestroySwapChain(*m_SwapChain);
        }
    }

    //

    void WindowManager::NewFrame()
    {
        auto& SwapchainInterface = *m_NriBridge.GetSwapChainInterface();
        m_BackBufferIndex        = SwapchainInterface.AcquireNextSwapChainTexture(*m_SwapChain);
    }

    void WindowManager::Present()
    {
        auto& SwapchainInterface = *m_NriBridge.GetSwapChainInterface();
        SwapchainInterface.QueuePresent(*m_SwapChain);
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
        uint8_t Index) const
    {
        return m_BackBuffers[Index];
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

    void WindowManager::RecreateSwapchain(
        nri::Device&       Device,
        nri::CommandQueue& GraphicsQueue)
    {
        auto& SwapchainInterface = *m_NriBridge.GetSwapChainInterface();

        m_DirtySwapChain = false;
        ReleaseBackBuffers();

        if (m_SwapChain)
        {
            SwapchainInterface.DestroySwapChain(*m_SwapChain);
            m_SwapChain = nullptr;
        }

        auto WindowSize   = m_Window.GetSize();
        auto WindowHandle = m_Window.GetHandle();

        nri::Window Window
        {
#if AME_PLATFORM_WINDOWS
            .windows = { glfwGetWin32Window(WindowHandle) }
#elif AME_PLATFORM_LINUX
            .x11 = {
                .dpy    = glfwGetX11Display(),
                .window = glfwGetX11Window(WindowHandle)
            }
#endif
        };

        nri::SwapChainDesc SwapChainDesc{
            .window               = Window,
            .commandQueue         = &GraphicsQueue,
            .width                = static_cast<nri::Dim_t>(WindowSize.x),
            .height               = static_cast<nri::Dim_t>(WindowSize.y),
            .textureNum           = static_cast<uint8_t>(m_BackBuffers.size()),
            .format               = m_SwapChainFormat,
            .verticalSyncInterval = m_VSyncEnabled
        };

        ThrowIfFailed(SwapchainInterface.CreateSwapChain(Device, SwapChainDesc, m_SwapChain), "Failed to create swapchain");
        CreateSwapChainViews();
    }

    void WindowManager::CreateSwapChainViews()
    {
        auto& SwapchainInterface = *m_NriBridge.GetSwapChainInterface();
        auto& CoreInterface      = *m_NriBridge.GetCoreInterface();

        uint32_t Count       = 0;
        auto     BackBuffers = SwapchainInterface.GetSwapChainTextures(*m_SwapChain, Count);
        m_BackBufferFormat   = CoreInterface.GetTextureDesc(*BackBuffers[0]).format;

        nri::Texture2DViewDesc ViewDesc{
            .viewType = nri::Texture2DViewType::COLOR_ATTACHMENT,
            .format   = m_BackBufferFormat
        };

        for (uint32_t i = 0; i < Count; i++)
        {
            ViewDesc.texture          = BackBuffers[i];
            m_BackBuffers[i].Resource = Texture(BackBuffers[i]);

            nri::Descriptor* View = nullptr;
            ThrowIfFailed(CoreInterface.CreateTexture2DView(ViewDesc, View), "Failed to create texture view of the swapchain texture");
            m_BackBuffers[i].View = RenderTargetResourceView(View);
        }
    }

    void WindowManager::ReleaseBackBuffers()
    {
        auto& CoreInterface = *m_NriBridge.GetCoreInterface();
        for (auto& BackBuffer : m_BackBuffers)
        {
            if (auto Descriptor = BackBuffer.View.Unwrap())
            {
                CoreInterface.DestroyDescriptor(*Descriptor);
                BackBuffer.View = {};
            }
        }
    }
} // namespace Ame::Rhi