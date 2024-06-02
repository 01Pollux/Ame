#pragma once

#include <Asset/Handler.hpp>
#include <Asset/Types/Gfx/TextureAsset.hpp>

namespace Ame
{
    namespace Plugins
    {
        class FreeImageInstance;
    } // namespace Plugins
    namespace Rhi
    {
        class Device;
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
            Rhi::Device&                          rhiDevice,
            Rhi::Staging::DeferredStagingManager& stagingManager,
            Plugins::FreeImageInstance&);

    public:
        AME_STANDARD_ASSET_HANDLER_BODY;

    private:
        /// <summary>
        /// Get the extension of a texture asset.
        /// </summary>
        [[nodiscard]] static String GetExtension(
            String&              path,
            const AssetMetaData& loaderData);

    private:
        Ref<Rhi::Device>                          m_Device;
        Ref<Rhi::Staging::DeferredStagingManager> m_StagingManager;
    };
} // namespace Ame::Asset::Gfx