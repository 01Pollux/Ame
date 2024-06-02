#pragma once

#include <Asset/Asset.hpp>
#include <Gfx/Cache/ShaderCache.hpp>

namespace Ame::Asset::Gfx
{
    class ShaderSourceAsset : public IAsset
    {
        using ShaderCache = Ame::Gfx::Cache::ShaderCache;

    public:
        class Handler;

    public:
        ShaderSourceAsset(
            ShaderCache& shaderCache,
            String       shaderSource,
            const Guid&  guid,
            String       path);

        /// <summary>
        /// Load the shader cache from settings
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> Load(
            const Rhi::ShaderCompileDesc& desc);

    private:
        Ref<ShaderCache> m_Cache;
        String           m_ShaderSource;
    };
} // namespace Ame::Asset::Gfx