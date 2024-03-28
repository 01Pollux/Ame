#include <Rhi/Texture.hpp>

#include "NriError.hpp"

namespace Ame::Rhi
{
    Texture::Texture(
        const TextureDesc& Desc)
    {
    }

    //

    void Texture::Release()
    {
    }

    void Texture::DeferRelease()
    {
    }

    //

    void Texture::SetName(
        const char* Name)
    {
    }

    const TextureDesc& Texture::GetDesc() const
    {
        static TextureDesc desc;
        return desc;
    }

    nri::Texture* Texture::Unwrap() const
    {
        return nullptr;
    }

    void* Texture::GetNative() const
    {
        return nullptr;
    }

    //

    ResourceView Texture::CreateShaderView(
        const TextureViewDesc& Desc) const
    {
        return ResourceView();
    }

    RenderTargetResourceView Texture::CreateRenderTargetView(
        const TextureViewDesc& Desc) const
    {
        return RenderTargetResourceView();
    }

    DepthStencilResourceView Texture::CreateDepthStencilView(
        const TextureViewDesc& Desc) const
    {
        return DepthStencilResourceView();
    }

    //

    ResourceView Texture::CreateView(
        const TextureViewDesc& Desc) const
    {
        return ResourceView();
    }
} // namespace Ame::Rhi