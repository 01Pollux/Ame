#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        Extern,
        DeviceImpl&   rhiDeviceImpl,
        nri::Texture* nriTexture) :
        m_Device(&rhiDeviceImpl),
        m_Texture(nriTexture),
        m_Owning(false)
    {
    }

    Texture::Texture(
        Extern,
        Device&       rhiDevice,
        nri::Texture* nriTexture) :
        m_Device(&rhiDevice.GetImpl()),
        m_Texture(nriTexture),
        m_Owning(false)
    {
    }

    Texture::Texture(
        Device&            rhiDevice,
        MemoryLocation     location,
        const TextureDesc& desc) :
        m_Device(&rhiDevice.GetImpl()),
        m_Texture(rhiDevice.Create(location, desc))
    {
    }

    //

    Texture::Texture(
        Texture&& other) noexcept :
        m_Device(std::exchange(other.m_Device, nullptr)),
        m_Texture(std::exchange(other.m_Texture, nullptr)),
        m_Owning(std::exchange(other.m_Owning, false))
    {
    }

    Texture& Texture::operator=(
        Texture&& other) noexcept
    {
        if (this != &other)
        {
            Release();

            m_Device  = std::exchange(other.m_Device, nullptr);
            m_Texture = std::exchange(other.m_Texture, nullptr);
            m_Owning  = std::exchange(other.m_Owning, false);
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
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.SetTextureDebugName(*m_Texture, Name);
    }

    const TextureDesc& Texture::GetDesc() const
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        return nriCore.GetTextureDesc(*m_Texture);
    }

    nri::Texture* const& Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative() const
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        return std::bit_cast<void*>(nriCore.GetTextureNativeObject(*m_Texture));
    }

    //

    Texture Texture::Borrow() const
    {
        return Texture(Extern{}, *m_Device, m_Texture);
    }

    bool Texture::IsOwning() const
    {
        return m_Owning;
    }

    //

    ResourceView Texture::CreateView(
        const TextureViewDesc& Desc) const
    {
        return ResourceView(m_Device, m_Device->CreateView(*m_Texture, Desc.Transform(*this)));
    }

    ShaderResourceView Texture::CreateShaderView(
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & (TextureViewType::AnyShaderResource | TextureViewType::AnyUnorderedAccess)) != TextureViewType::None,
                          "Texture view type must be a shader resource type or an unordered access type.");
#endif
        return ShaderResourceView(m_Device, m_Device->CreateView(*m_Texture, Desc.Transform(*this)));
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
        MemoryLocation     location,
        const TextureDesc& desc)
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        auto nriTexture = m_Impl->m_MemoryAllocator.CreateTexture(location, desc);
        m_Impl->BeginTracking(nriTexture, { nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL });

        return nriTexture;
    }

    //

    void DeviceImpl::BeginTracking(
        nri::Texture*          nriTexture,
        nri::AccessLayoutStage initialState)
    {
        m_ResourceStateTracker.BeginTracking(*m_NRI.GetCoreInterface(), nriTexture, initialState);
    }

    void DeviceImpl::EndTracking(
        nri::Texture* nriTexture)
    {
        m_ResourceStateTracker.EndTracking(nriTexture);
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
        nri::Texture& nriTexture)
    {
        m_Impl->EndTracking(&nriTexture);
        m_Impl->Release(nriTexture);
    }

    void DeviceImpl::Release(
        nri::Texture& nriTexture)
    {
        EndTracking(&nriTexture);
        m_FrameManager.DeferRelease(nriTexture);
    }
} // namespace Ame::Rhi