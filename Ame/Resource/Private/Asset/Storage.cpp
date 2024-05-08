#include <Asset/Storages/DefaultStorage.hpp>
#include <Asset/Pack.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    void IStorage::ExportAll()
    {
        std::vector<Co::result<void>> Futures;

        for (auto& Package : GetPackages())
        {
            Futures.emplace_back(Package->Export());
        }

        for (auto& Future : Futures)
        {
            try
            {
                Future.get();
            }
            catch (const std::exception& Exception)
            {
                Log::Asset().Error("Failed to export packages: {}", Exception.what());
            }
        }
    }

    //

    IAssetPackage* IStorage::FindPackage(
        const Handle&       AssetGuid,
        const PackageFlags& Flags)
    {
        for (auto& Package : GetPackages(Flags))
        {
            if (Package->ContainsAsset(AssetGuid))
            {
                return Package;
            }
        }
        return nullptr;
    }

    auto IStorage::FindAsset(
        const String&       Path,
        const PackageFlags& Flags) -> PackageAndAsset
    {
        for (auto& Package : GetPackages(Flags))
        {
            if (auto Asset = Package->FindAsset(Path); !Asset.is_nil())
            {
                return { Package, Asset };
            }
        }
        return { nullptr, Asset::Handle::Null };
    }

    auto IStorage::FindAssets(
        const std::regex&   PathRegex,
        const PackageFlags& Flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& Package : GetPackages(Flags))
        {
            for (auto& Asset : Package->FindAssets(PathRegex))
            {
                co_yield { Package, Asset };
            }
        }
    }
} // namespace Ame::Asset