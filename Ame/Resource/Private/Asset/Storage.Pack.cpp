#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    IAssetPackage* Storage::Mount(
        UPtr<IAssetPackage> Package)
    {
#ifndef AME_RELEASE
        if (&Package->GetStorage() != this)
        {
            Log::Asset().Error("Trying to mount an asset that part of this storage");
            return nullptr;
        }
#endif

        return m_Packages.emplace_back(std::move(Package)).get();
    }

    void Storage::Unmount(
        IAssetPackage* Package)
    {
        std::erase_if(
            m_Packages, [Package](const auto& CurPackage)
            { return CurPackage.get() == Package; });
    }

    //

    void Storage::ExportAll()
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

    Co::generator<IAssetPackage*> Storage::GetPackages(
        const PackageFlags& Flags)
    {
        using namespace EnumBitOperators;

        auto Iter = m_Packages.begin();
        if ((Flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            co_yield Iter->get();
        }

        ++Iter;
        if ((Flags & PackageFlags::Disk) == PackageFlags::Disk) [[likely]]
        {
            for (; Iter != m_Packages.end(); ++Iter)
            {
                co_yield Iter->get();
            }
        }
    }

    //

    IAssetPackage* Storage::FindPackage(
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
} // namespace Ame::Asset