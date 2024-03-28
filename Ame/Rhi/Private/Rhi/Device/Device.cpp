#include <Rhi/Device.hpp>
#include "DeviceImpl.hpp"

#include "../NriError.hpp"

namespace Ame::Rhi
{
    Device::Device(
        const DeviceCreateDesc& Desc) :
        m_Impl(std::make_unique<Impl>(Desc))
    {
    }

    Device::~Device() = default;

    //

    GraphicsAPI Device::GetGraphicsAPI() const
    {
        return m_Impl->GetGraphicsAPI();
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

    //

    bool Device::IsHeadless() const
    {
        return m_Impl->IsHeadless();
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

    Backbuffer Device::GetBackbuffer(
        uint8_t Index) const
    {
        return m_Impl->GetBackbuffer(Index);
    }

    Backbuffer Device::GetBackbuffer() const
    {
        return m_Impl->GetBackbuffer();
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
} // namespace Ame::Rhi