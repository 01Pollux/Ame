#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Texture::Texture(
        DeviceResourceAllocator& allocator,
        nri::Texture*            texture) :
        m_Allocator(&allocator),
        m_Texture(texture)
    {
    }

    //

    void Texture::Release(
        bool defer)
    {
        if (m_Texture)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_Texture);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyTexture(*m_Texture);
            }
            m_Texture = nullptr;
        }
    }

    void Texture::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetTextureDebugName(*m_Texture, name);
    }

    const TextureDesc& Texture::GetDesc() const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        return nriCore.GetTextureDesc(*m_Texture);
    }

    nri::Texture* const& Texture::Unwrap() const
    {
        return m_Texture;
    }

    void* Texture::GetNative() const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        return std::bit_cast<void*>(nriCore.GetTextureNativeObject(*m_Texture));
    }
} // namespace Ame::Rhi