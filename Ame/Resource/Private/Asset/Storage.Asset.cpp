#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    Co::result<void> Storage::SaveAsset(
        const AddDesc& Desc)
    {
        AME_LOG_ASSERT(Log::Asset(), Desc.Asset != nullptr, "Asset is null");

        using namespace EnumBitOperators;

        IAssetPackage* Package = Desc.PreferredPackage;
        if ((Desc.Flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            Package = m_Packages.front().get();
        }
        else if (!Package)
        {
            AME_LOG_ASSERT(Log::Asset(), m_Packages.size() > 1, "No packages mounted");
            Package = std::next(m_Packages.begin())->get();
        }
#if AME_DEBUG
        else
        {
            if (std::ranges::find_if(
                    m_Packages, [Package](const auto& CurPackage)
                    { return CurPackage.get() == Package; }) == m_Packages.end())
            {
                AME_LOG_ASSERT(Log::Asset(), false, "Package not mounted");
            }
        }
#endif

        return Package->SaveAsset(Desc.Asset);
    }

    void Storage::RemoveAsset(
        const Handle& AssetGuid)
    {
        for (auto& Package : m_Packages)
        {
            if (Package->RemoveAsset(AssetGuid))
            {
                return;
            }
        }

        Log::Asset().Warning("Asset '{}' not found", AssetGuid.ToString());
    }

    //

    auto Storage::GetAllAssets(
        const PackageFlags& Flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& Packages : GetPackages(Flags))
        {
            for (auto& AssetGuid : Packages->GetAssets())
            {
                co_yield { Packages, AssetGuid };
            }
        }
    }

    //

    auto Storage::FindAsset(
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

    auto Storage::FindAssets(
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