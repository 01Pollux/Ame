#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

namespace Ame::Asset::Gfx
{
    ShaderSourceAsset::ShaderSourceAsset(
        ShaderCache& shaderCache,
        String       shaderSource,
        const Guid&  guid,
        String       path) :
        IAsset(guid, std::move(path)),
        m_Cache(shaderCache),
        m_ShaderSource(std::move(shaderSource))
    {
    }

    Co::result<Rhi::ShaderBytecode> ShaderSourceAsset::Load(
        const Rhi::ShaderCompileDesc& desc)
    {
        return m_Cache.get().Load({ desc, m_ShaderSource });
    }
} // namespace Ame::Asset::Gfx