#pragma once

#include <Asset/Handler.hpp>
#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

namespace Ame::Asset::Gfx
{
    class ShaderSourceAsset::Handler : public IAssetHandler
    {
    public:
        static constexpr size_t UID = 19456857095889080;

    public:
        Handler(
            ShaderCache& shaderCache);

    public:
        AME_STANDARD_ASSET_HANDLER_BODY;

    private:
        Ref<ShaderCache> m_Cache;
    };
} // namespace Ame::Asset::Gfx