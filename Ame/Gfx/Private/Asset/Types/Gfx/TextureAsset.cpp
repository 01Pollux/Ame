#include <Asset/Types/Gfx/TextureAsset.hpp>

namespace Ame::Asset::Gfx
{
    TextureAsset::TextureAsset(
        const Ptr<Rhi::Texture>& texture,
        ImageFormat              Format,
        const Guid&              guid,
        String                   path) :
        IAsset(guid, std::move(path)),
        m_Texture(texture),
        m_Format(Format)
    {
    }

    const Ptr<Rhi::Texture>& TextureAsset::GetTexture() const
    {
        return m_Texture;
    }

    ImageFormat TextureAsset::GetFormat() const
    {
        return m_Format;
    }
} // namespace Ame::Asset::Gfx