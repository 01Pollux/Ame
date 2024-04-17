#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        Extern,
        DeviceImpl&   RhiDevice,
        nri::Texture* RhiTexture) :
        m_Device(&RhiDevice),
        m_Texture(RhiTexture),
        m_Owning(false)
    {
    }

    Texture::Texture(
        Extern,
        Device&       RhiDevice,
        nri::Texture* RhiTexture) :
        m_Device(&RhiDevice.GetImpl()),
        m_Texture(RhiTexture),
        m_Owning(false)
    {
    }

    Texture::Texture(
        Device&            RhiDevice,
        MemoryLocation     Location,
        const TextureDesc& Desc) :
        m_Device(&RhiDevice.GetImpl()),
        m_Texture(RhiDevice.Create(Location, Desc))
    {
    }

    Texture::Texture(
        Texture&& Other) noexcept :
        m_Device(std::exchange(Other.m_Device, nullptr)),
        m_Texture(std::exchange(Other.m_Texture, nullptr)),
        m_Owning(std::exchange(Other.m_Owning, false))
    {
    }

    Texture& Texture::operator=(
        Texture&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();

            m_Device  = std::exchange(Other.m_Device, nullptr);
            m_Texture = std::exchange(Other.m_Texture, nullptr);
            m_Owning  = std::exchange(Other.m_Owning, false);
        }

        return *this;
    }

    Texture::~Texture()
    {
        Release();
    }

    //

    void Texture::SetName(
        const char* Name) const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetTextureDebugName(*m_Texture, Name);
    }

    const TextureDesc& Texture::GetDesc() const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return NriCore.GetTextureDesc(*m_Texture);
    }

    nri::Texture* Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative() const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return std::bit_cast<void*>(NriCore.GetTextureNativeObject(*m_Texture));
    }

    //

    ResourceView Texture::CreateShaderView(
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & (TextureViewType::AnyShaderResource | TextureViewType::AnyUnorderedAccess)) != TextureViewType::None,
                          "Texture view type must be a shader resource type or an unordered access type.");
#endif
        return ResourceView(m_Device, m_Device->CreateView(*m_Texture, Desc.Transform(*this)));
    }

    RenderTargetResourceView Texture::CreateRenderTargetView(
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & TextureViewType::AnyRenderTarget) != TextureViewType::None,
                          "Texture view type must be a render target type.");
#endif
        return RenderTargetResourceView(m_Device, m_Device->CreateView(*m_Texture, Desc.Transform(*this)));
    }

    DepthStencilResourceView Texture::CreateDepthStencilView(
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & TextureViewType::AnyDepthStencil) != TextureViewType::None,
                          "Texture view type must be a depth stencil type.");
#endif
        return DepthStencilResourceView(m_Device, m_Device->CreateView(*m_Texture, Desc.Transform(*this)));
    }

    //

    nri::Texture* Device::Create(
        MemoryLocation     Location,
        const TextureDesc& Desc)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto NriTexture = m_Impl->m_MemoryAllocator.CreateTexture(Location, Desc);
        m_Impl->BeginTracking(NriTexture, { nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL });

        return NriTexture;
    }

    //

    void DeviceImpl::BeginTracking(
        nri::Texture*          Texture,
        nri::AccessLayoutStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(*m_NRI.GetCoreInterface(), Texture, InitialState);
    }

    void DeviceImpl::EndTracking(
        nri::Texture* Texture)
    {
        m_ResourceStateTracker.EndTracking(Texture);
    }

    //

    void Texture::Release()
    {
        if (!m_Owning || !m_Device)
        {
            return;
        }

        m_Device->Release(*m_Texture);
        m_Texture = nullptr;
        m_Owning  = false;
    }

    void Device::Release(
        nri::Texture& NriTexture)
    {
        m_Impl->EndTracking(&NriTexture);
        m_Impl->Release(NriTexture);
    }

    void DeviceImpl::Release(
        nri::Texture& NriTexture)
    {
        m_FrameManager.DeferRelease(NriTexture);
    }
} // namespace Ame::Rhi