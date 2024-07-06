#pragma once

#include <Asset/Handler.hpp>
#include <Asset/Types/Gfx/TextureAsset.hpp>

namespace Ame
{
    namespace Extensions
    {
        class FreeImageInstance;
    } // namespace Extensions
    namespace Rhi
    {
        class RhiDevice;
        namespace Staging
        {
            class DeferredStagingManager;
        } // namespace Staging
    }     // namespace Rhi
} // namespace Ame

namespace Ame::Asset::Gfx
{
    class TextureAsset::Handler : public IAssetHandler
    {
    public:
        static constexpr size_t UID = 17988892139631915;

    public:
        Handler(
            Rhi::Device& rhiDevice,
            Extensions::FreeImageInstance&);

    public:
        AME_STANDARD_ASSET_HANDLER_BODY;

    private:
        Ref<Rhi::Device>                          m_Device;
    };
} // namespace Ame::Asset::Gfx