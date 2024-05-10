#pragma once

#include <Asset/Handler.hpp>
#include <Asset/Types/Gfx/ShaderSourceAsset.hpp>

namespace Ame::Asset::Gfx
{
    class ShaderSourceAsset::Handler : public IAssetHandler
    {
    public:
        Handler(
            ShaderCache& Cache);

    public:
        AME_STANDARD_ASSET_HANDLER_BODY;

    private:
        Ref<ShaderCache> m_Cache;
    };
} // namespace Ame::Asset::Gfx