#include <Asset/Handler.hpp>

#include <cereal/cereal.hpp>

namespace Ame::Asset
{
    bool DefaultAssetHandler::CanHandle(
        const Ptr<IAsset>&)
    {
        return true;
    }

    Co::result<Ptr<IAsset>> DefaultAssetHandler::Load(
        AssetHandlerLoadDesc& loadDesc)
    {
        co_await Co::resume_on(loadDesc.BackgroundExecutor);

        BinaryIArchiver archiver(loadDesc.Stream);

        co_await Co::resume_on(loadDesc.ForegroundExecutor);
    }

    Co::result<void> DefaultAssetHandler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        co_await Co::resume_on(saveDesc.BackgroundExecutor);

        BinaryOArchiver archiver(saveDesc.Stream);

        co_await Co::resume_on(saveDesc.ForegroundExecutor);
    }
} // namespace Ame::Asset