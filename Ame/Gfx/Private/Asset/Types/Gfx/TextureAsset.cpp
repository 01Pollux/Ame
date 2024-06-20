#include <Asset/Types/Gfx/TextureAsset.hpp>

namespace Ame::Asset::Gfx
{
    TextureAsset::TextureAsset(
        const Ptr<Rhi::ScopedTexture>& texture,
        ImageFormat                    Format,
        const Guid&                    guid,
        String                         path) :
        IAsset(guid, std::move(path)),
        m_Texture(texture),
        m_Format(Format)
    {
    }

    const Ptr<Rhi::ScopedTexture>& TextureAsset::GetTexture() const
    {
        return m_Texture;
    }

    ImageFormat TextureAsset::GetFormat() const
    {
        return m_Format;
    }
} // namespace Ame::Asset::Gfx