#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        nri::CoreInterface& nriCore,
        nri::Texture*       texture) :
        m_NriCore(&nriCore),
        m_Texture(texture)
    {
    }

    //

    void Texture::SetName(
        const char* name) const
    {
        m_NriCore->SetTextureDebugName(*m_Texture, name);
    }

    const TextureDesc& Texture::GetDesc() const
    {
        return m_NriCore->GetTextureDesc(*m_Texture);
    }

    nri::Texture* const& Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative() const
    {
        return std::bit_cast<void*>(m_NriCore->GetTextureNativeObject(*m_Texture));
    }

    //

    void ScopedTexture::Release()
    {
        m_Allocator->Release(*this).wait();
    }
} // namespace Ame::Rhi