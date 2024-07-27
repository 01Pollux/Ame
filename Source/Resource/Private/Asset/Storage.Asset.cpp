#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    Co::result<void> Storage::SaveAsset(
        const AddDesc& desc)
    {
        AME_LOG_ASSERT(Log::Asset(), desc.Asset != nullptr, "Asset is null");

        using namespace EnumBitOperators;

        IAssetPackage* package = desc.PreferredPackage;
        if ((desc.Flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            package = m_Packages.front();
        }
        else if (!package)
        {
            AME_LOG_ASSERT(Log::Asset(), m_Packages.size() > 1, "No packages mounted");
            package = *std::next(m_Packages.begin());
        }
#ifdef AME_DEBUG
        else
        {
            if (std::ranges::find_if(
                    m_Packages, [package](const auto& curPackage)
                    { return curPackage == package; }) == m_Packages.end())
            {
                AME_LOG_ASSERT(Log::Asset(), false, "Package not mounted");
            }
        }
#endif

        return package->SaveAsset(desc.Asset);
    }

    void Storage::RemoveAsset(
        const UId& uid)
    {
        for (auto& package : m_Packages)
        {
            if (package->RemoveAsset(uid))
            {
                return;
            }
        }

        Log::Asset().Warning("Asset '{}' not found", UIdUtils::ToString(uid));
    }

    //

    auto Storage::GetAllAssets(
        const PackageFlags& flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& package : GetPackages(flags))
        {
            for (auto& uid : package->GetAssets())
            {
                co_yield { package, uid };
            }
        }
    }

    //

    auto Storage::FindAsset(
        const String&       path,
        const PackageFlags& flags) -> PackageAndAsset
    {
        PackageAndAsset result;
        for (auto& package : GetPackages(flags))
        {
            if (auto uid = package->FindAsset(path); UIdUtils::IsNull(uid))
            {
                result = { package, uid };
                break;
            }
        }
        return result;
    }

    auto Storage::FindAssets(
        const std::regex&   pathRegex,
        const PackageFlags& flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& package : GetPackages(flags))
        {
            for (auto& uid : package->FindAssets(pathRegex))
            {
                co_yield { package, uid };
            }
        }
    }
} // namespace Ame::Asset