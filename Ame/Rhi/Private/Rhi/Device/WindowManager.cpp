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
        SwapChainFormat Format)
    {
        switch (Format)
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
        DeviceImpl&             RhiDevice,
        const DeviceCreateDesc& Desc) :
        m_Device(RhiDevice),
        m_Window(Desc.Window->Window),
        m_SwapChainFormat(ToSwapchainFormat(Desc.Window->Format)),
        m_BackBuffers(Desc.Window->BackbufferCount),
        m_VSyncEnabled(Desc.EnableVSync)
    {
        RecreateSwapchain();
        m_Window.OnWindowSizeChanged().Listen(
            [this](const auto&, const auto&)
            {
                m_DirtySwapChain = true;
            });
    }

    WindowManager::~WindowManager()
    {
        ReleaseBackBuffers();
        if (m_SwapChain)
        {
            auto& Nri          = m_Device.GetNRI();
            auto& NriSwapchain = *Nri.GetSwapChainInterface();

            NriSwapchain.DestroySwapChain(*m_SwapChain);
        }
    }

    //

    void WindowManager::NewFrame()
    {
        auto& Nri          = m_Device.GetNRI();
        auto& NriSwapchain = *Nri.GetSwapChainInterface();

        m_BackBufferIndex = NriSwapchain.AcquireNextSwapChainTexture(*m_SwapChain);
    }

    void WindowManager::Present()
    {
        auto& Nri          = m_Device.GetNRI();
        auto& NriSwapchain = *Nri.GetSwapChainInterface();

        NriSwapchain.QueuePresent(*m_SwapChain);
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

    void WindowManager::RecreateSwapchain()
    {
        auto& Nri           = m_Device.GetNRI();
        auto& NriSwapchain  = *Nri.GetSwapChainInterface();
        auto& GraphicsQueue = m_Device.GetQueue();

        m_DirtySwapChain = false;
        ReleaseBackBuffers();

        if (m_SwapChain)
        {
            NriSwapchain.DestroySwapChain(*m_SwapChain);
            m_SwapChain = nullptr;
        }

        auto WindowSize   = m_Window.GetSize();
        auto WindowHandle = m_Window.GetHandle();

        nri::Window Window{
#if AME_PLATFORM_WINDOWS
            .windows = { glfwGetWin32Window(WindowHandle) }
#elif AME_PLATFORM_LINUX
            .x11 = {
                .dpy    = glfwGetX11Display(),
                .window = glfwGetX11Window(WindowHandle) }
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

        ThrowIfFailed(NriSwapchain.CreateSwapChain(m_Device.GetDevice(), SwapChainDesc, m_SwapChain), "Failed to create swapchain");
        CreateSwapChainViews();
    }

    void WindowManager::CreateSwapChainViews()
    {
        auto& Nri          = m_Device.GetNRI();
        auto& NriSwapchain = *Nri.GetSwapChainInterface();
        auto& NriCore      = *Nri.GetCoreInterface();

        uint32_t Count       = 0;
        auto     BackBuffers = NriSwapchain.GetSwapChainTextures(*m_SwapChain, Count);
        m_BackBufferFormat   = NriCore.GetTextureDesc(*BackBuffers[0]).format;

        nri::Texture2DViewDesc ViewDesc{
            .viewType = nri::Texture2DViewType::COLOR_ATTACHMENT,
            .format   = m_BackBufferFormat
        };

        for (uint32_t i = 0; i < Count; i++)
        {
            ViewDesc.texture          = BackBuffers[i];
            m_BackBuffers[i].Resource = Texture(Texture::Extern{}, m_Device, BackBuffers[i]);

            nri::Descriptor* View = nullptr;
            ThrowIfFailed(NriCore.CreateTexture2DView(ViewDesc, View), "Failed to create texture view of the swapchain texture");
            m_BackBuffers[i].View = RenderTargetResourceView(ResourceView::Extern{}, m_Device, View);
        }
    }

    void WindowManager::ReleaseBackBuffers()
    {
        auto& Nri     = m_Device.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        for (auto& BackBuffer : m_BackBuffers)
        {
            if (auto Descriptor = BackBuffer.View.Unwrap())
            {
                NriCore.DestroyDescriptor(*Descriptor);
                BackBuffer.View = {};
            }
        }
    }
} // namespace Ame::Rhi