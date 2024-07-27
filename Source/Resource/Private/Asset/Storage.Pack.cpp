#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    void Storage::Mount(
        Ptr<IAssetPackage> package)
    {
#ifndef AME_RELEASE
        if (&package->GetStorage() != this)
        {
            Log::Asset().Error("Trying to mount an asset that part of this storage");
            return;
        }
#endif

        m_Packages.emplace_back(std::move(package));
    }

    void Storage::Unmount(
        IAssetPackage* package)
    {
        std::erase_if(
            m_Packages, [package](const auto& CurPackage)
            { return CurPackage == package; });
    }

    //

    void Storage::ExportAll()
    {
        std::vector<Co::result<void>> tasks;

        for (auto& package : GetPackages())
        {
            tasks.emplace_back(package->Export());
        }

        for (auto& Future : tasks)
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

    Co::generator<Ptr<IAssetPackage>> Storage::GetPackages(
        const PackageFlags& flags)
    {
        using namespace EnumBitOperators;

        auto iter = m_Packages.begin();
        if ((flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            co_yield *iter;
        }

        ++iter;
        if ((flags & PackageFlags::Disk) == PackageFlags::Disk) [[likely]]
        {
            for (; iter != m_Packages.end(); ++iter)
            {
                co_yield *iter;
            }
        }
    }

    //

    IAssetPackage* Storage::FindPackage(
        const Guid&         guid,
        const PackageFlags& flags)
    {
        for (auto& package : GetPackages(flags))
        {
            if (package->ContainsAsset(guid))
            {
                return package;
            }
        }
        return nullptr;
    }
} // namespace Ame::Asset