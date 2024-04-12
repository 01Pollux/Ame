#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        Device&            RhiDevice,
        MemoryLocation     Location,
        const TextureDesc& Desc) :
        m_Texture(RhiDevice.Create(Location, Desc))
    {
    }

    void Texture::Release(
        Device& RhiDevice,
        bool    Defer)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Texture != nullptr, "Texture was already released.");
        RhiDevice.Release(*m_Texture, Defer);
        m_Texture = nullptr;
    }

    //

    void Texture::SetName(
        Device&     RhiDevice,
        const char* Name) const
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetTextureDebugName(*m_Texture, Name);
    }

    const TextureDesc& Texture::GetDesc(
        Device& RhiDevice) const
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return NriCore.GetTextureDesc(*m_Texture);
    }

    nri::Texture* Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative(
        Device& RhiDevice) const
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return std::bit_cast<void*>(NriCore.GetTextureNativeObject(*m_Texture));
    }

    //

    ResourceView Texture::CreateShaderView(
        Device&                RhiDevice,
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & (TextureViewType::AnyShaderResource | TextureViewType::AnyUnorderedAccess)) != TextureViewType::None,
                          "Texture view type must be a shader resource type or an unordered access type.");
#endif
        return ResourceView(RhiDevice.CreateView(*m_Texture, Desc));
    }

    RenderTargetResourceView Texture::CreateRenderTargetView(
        Device&                RhiDevice,
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & TextureViewType::AnyRenderTarget) != TextureViewType::None,
                          "Texture view type must be a render target type.");
#endif
        return RenderTargetResourceView(RhiDevice.CreateView(*m_Texture, Desc));
    }

    DepthStencilResourceView Texture::CreateDepthStencilView(
        Device&                RhiDevice,
        const TextureViewDesc& Desc) const
    {
#ifdef AME_DEBUG
        using namespace EnumBitOperators;
        Log::Rhi().Assert((Desc.Type & TextureViewType::AnyDepthStencil) != TextureViewType::None,
                          "Texture view type must be a depth stencil type.");
#endif
        return DepthStencilResourceView(RhiDevice.CreateView(*m_Texture, Desc));
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

    void Device::Release(
        nri::Texture& NriTexture,
        bool          Defer)
    {
        m_Impl->EndTracking(&NriTexture);
        m_Impl->Release(NriTexture, Defer);
    }

    void DeviceImpl::Release(
        nri::Texture& NriTexture,
        bool          Defer)
    {
        if (Defer)
        {
            m_FrameManager.DeferRelease(NriTexture);
        }
        else
        {
            m_MemoryAllocator.Release(&NriTexture);
        }
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
} // namespace Ame::Rhi