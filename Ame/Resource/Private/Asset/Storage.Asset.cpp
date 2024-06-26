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
            package = m_Packages.front().get();
        }
        else if (!package)
        {
            AME_LOG_ASSERT(Log::Asset(), m_Packages.size() > 1, "No packages mounted");
            package = std::next(m_Packages.begin())->get();
        }
#if AME_DEBUG
        else
        {
            if (std::ranges::find_if(
                    m_Packages, [package](const auto& curPackage)
                    { return curPackage.get() == package; }) == m_Packages.end())
            {
                AME_LOG_ASSERT(Log::Asset(), false, "Package not mounted");
            }
        }
#endif

        return package->SaveAsset(desc.Asset);
    }

    void Storage::RemoveAsset(
        const Guid& guid)
    {
        for (auto& package : m_Packages)
        {
            if (package->RemoveAsset(guid))
            {
                return;
            }
        }

        Log::Asset().Warning("Asset '{}' not found", guid.ToString());
    }

    //

    auto Storage::GetAllAssets(
        const PackageFlags& flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& package : GetPackages(flags))
        {
            for (auto& guid : package->GetAssets())
            {
                co_yield { package, guid };
            }
        }
    }

    //

    auto Storage::FindAsset(
        const String&       path,
        const PackageFlags& flags) -> PackageAndAsset
    {
        for (auto& package : GetPackages(flags))
        {
            if (auto guid = package->FindAsset(path); !guid.is_nil())
            {
                return { package, guid };
            }
        }
        return { nullptr, Guid::c_Null };
    }

    auto Storage::FindAssets(
        const std::regex&   pathRegex,
        const PackageFlags& flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& package : GetPackages(flags))
        {
            for (auto& guid : package->FindAssets(pathRegex))
            {
                co_yield { package, guid };
            }
        }
    }
} // namespace Ame::Asset