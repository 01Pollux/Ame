#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/Device.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        Device&            RhiDevice,
        const TextureDesc& Desc) :
        m_Texture(RhiDevice.Create(Desc))
    {
    }

    //

    void Texture::Release(
        Device& RhiDevice)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Texture != nullptr, "Texture was already released.");
        RhiDevice.Release(*m_Texture, false);
        m_Texture = nullptr;
    }

    void Texture::DeferRelease(
        Device& RhiDevice)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Texture != nullptr, "Texture was already released.");
        RhiDevice.Release(*m_Texture, true);
        m_Texture = nullptr;
    }

    //

    void Texture::SetName(
        Device&     RhiDevice,
        const char* Name) const
    {
        RhiDevice.SetName(*m_Texture, Name);
    }

    const TextureDesc& Texture::GetDesc(
        Device& RhiDevice) const
    {
        return RhiDevice.GetDesc(*m_Texture);
    }

    nri::Texture* Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative(
        Device& RhiDevice) const
    {
        return RhiDevice.GetNative(*m_Texture);
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
} // namespace Ame::Rhi