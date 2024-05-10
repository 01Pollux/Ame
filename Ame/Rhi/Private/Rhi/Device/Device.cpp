#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/Device/Device.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    Device::Device() = default;
    Device::Device(
        const DeviceCreateDesc& Desc) :
        m_Impl(std::make_unique<DeviceImpl>(Desc))
    {
        m_DrawIndexedCommandSize = GetDesc().isDrawParametersEmulationEnabled ? sizeof(Rhi::DrawIndexedBaseDesc) : sizeof(Rhi::DrawIndexedDesc);
    }

    Device::Device(Device&&)            = default;
    Device& Device::operator=(Device&&) = default;

    Device::~Device()
    {
        if (m_Impl)
        {
            CleanupCache();
            m_Impl = nullptr;
        }
    }

    //

    void Device::CleanupCheck()
    {
#ifndef AME_DIST
        DeviceImpl::CleanupCheck();
#endif
    }

    //

    GraphicsAPI Device::GetGraphicsAPI() const
    {
        return m_Impl ? m_Impl->GetGraphicsAPI() : GraphicsAPI::Null;
    }

    const char* Device::GetGraphicsAPIName() const
    {
        switch (GetGraphicsAPI())
        {
        case GraphicsAPI::Null:
            return "null";
        case GraphicsAPI::DirectX12:
            return "d3d12";
        case GraphicsAPI::Vulkan:
            return "vk";
        default:
            std::unreachable();
        }
    }

    const DeviceDesc& Device::GetDesc() const
    {
        return m_Impl->GetDesc();
    }

    uint64_t Device::GetFrameCount() const
    {
        return m_Impl->GetFrameCount();
    }

    uint8_t Device::GetFrameIndex() const
    {
        return m_Impl->GetFrameIndex();
    }

    uint8_t Device::GetFrameCountInFlight() const
    {
        return m_Impl->GetFrameCountInFlight();
    }

    uint32_t Device::GetDrawIndexedCommandSize() const
    {
        return m_DrawIndexedCommandSize;
    }

    //

    const Math::Color4& Device::GetClearColor() const noexcept
    {
        return m_Impl->GetClearColor();
    }

    void Device::SetClearColor(
        const Math::Color4& Color)
    {
        m_Impl->SetClearColor(Color);
    }

    BackbufferClearType Device::GetBackbufferClearType() const noexcept
    {
        return m_Impl->GetBackbufferClearType();
    }

    void Device::SetBackbufferClearType(
        BackbufferClearType Type)
    {
        m_Impl->SetBackbufferClearType(Type);
    }

    //

    bool Device::IsHeadless() const
    {
        return m_Impl && m_Impl->IsHeadless();
    }

    Windowing::Window& Device::GetWindow() const
    {
        return m_Impl->GetWindow();
    }

    uint8_t Device::GetBackbufferCount() const
    {
        return m_Impl->GetBackbufferCount();
    }

    uint8_t Device::GetBackbufferIndex() const
    {
        return m_Impl->GetBackbufferIndex();
    }

    const Backbuffer& Device::GetBackbuffer(
        uint8_t Index) const
    {
        return m_Impl->GetBackbuffer(Index);
    }

    const Backbuffer& Device::GetBackbuffer() const
    {
        return m_Impl->GetBackbuffer();
    }

    const TextureDesc& Device::GetBackBufferDesc() const
    {
        return GetBackbuffer().Resource.GetDesc();
    }

    //

    bool Device::IsVSyncEnabled() const noexcept
    {
        return m_Impl->IsHeadless();
    }

    void Device::SetVSyncEnabled(
        bool State)
    {
        m_Impl->SetVSyncEnabled(State);
    }

    //

    bool Device::ProcessEvents()
    {
        return m_Impl->ProcessEvents();
    }

    void Device::BeginFrame()
    {
        m_Impl->BeginFrame();
    }

    void Device::EndFrame()
    {
        m_Impl->EndFrame();
    }

    //

    void Device::WaitIdle()
    {
        m_Impl->WaitIdle();
    }

    void Device::CleanupCache()
    {
        m_Impl->CleanupCache();
    }

    //

    DeviceImpl& Device::GetImpl() const
    {
        return *m_Impl;
    }
} // namespace Ame::Rhi