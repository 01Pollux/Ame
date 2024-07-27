#include <Asset/Handler.hpp>
#include <rttr/type.h>

#include <Log/Wrapper.hpp>

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
        auto typeName = loadDesc.LoaderData.get().get<String>("Type");
        auto type     = rttr::type::get_by_name(typeName.c_str());

        // check if it's base of IAsset
        if (!type.is_derived_from<IAsset>())
        {
            co_return nullptr;
        }

        // create instance of asset and wrap it in shared_ptr
        Ptr asset(type.create().get_value<IAsset*>());
        if (!asset)
        {
            Log::Asset().Warning("Failed to create asset of type: {}", typeName);
            co_return asset;
        }

        co_await Co::resume_on(loadDesc.BackgroundExecutor);

        BinaryIArchiver archiver(loadDesc.Stream);
        asset->Deserialize(archiver);

        co_await Co::resume_on(loadDesc.ForegroundExecutor);

        co_return asset;
    }

    Co::result<void> DefaultAssetHandler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        co_await Co::resume_on(saveDesc.BackgroundExecutor);

        BinaryOArchiver archiver(saveDesc.Stream);
        saveDesc.Asset->Serialize(archiver);

        co_await Co::resume_on(saveDesc.ForegroundExecutor);

        auto typeName = rttr::type::get(*saveDesc.Asset).get_name().to_string();
        saveDesc.LoaderData.get().put("Type", typeName);
    }
} // namespace Ame::Asset