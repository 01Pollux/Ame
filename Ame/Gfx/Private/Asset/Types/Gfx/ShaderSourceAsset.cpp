#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

namespace Ame::Asset::Gfx
{
    ShaderSourceAsset::ShaderSourceAsset(
        ShaderCache&  Cache,
        String        ShaderSource,
        const Handle& AssetGuid,
        String        Path) :
        IAsset(AssetGuid, std::move(Path)),
        m_Cache(Cache),
        m_ShaderSource(std::move(ShaderSource))
    {
    }

    Co::result<Rhi::ShaderBytecode> ShaderSourceAsset::Load(
        const Rhi::ShaderCompileDesc& Desc)
    {
        return m_Cache.get().Load(m_ShaderSource, Desc);
    }
} // namespace Ame::Asset::Gfx