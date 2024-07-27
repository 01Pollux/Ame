#include <Asset/Storage.hpp>
#include <Asset/Manager.hpp>
#include <Asset/Pack.hpp>
#include <Asset/Exception.hpp>

#include <Log/Logger.hpp>

namespace Ame::Asset
{
    Co::result<Ptr<IAsset>> Manager::LoadAsync(
        IAssetPackage* package,
        Guid           guid,
        bool           loadTemp)
    {
        if (package->ContainsAsset(guid)) [[likely]]
        {
            co_return package->LoadAsset(guid, loadTemp);
        }

        co_return nullptr;
    }

    Co::result<Ptr<IAsset>> Manager::LoadAsync(
        Guid guid,
        bool loadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->ContainsAsset(guid))
            {
                co_return package->LoadAsset(guid, loadTemp);
            }
        }

        co_return nullptr;
    }

    Ptr<IAsset> Manager::Load(
        const Guid& guid,
        bool        loadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->ContainsAsset(guid))
            {
                return package->LoadAsset(guid, loadTemp);
            }
        }

        return {};
    }

    Ptr<IAsset> Manager::Load(
        IAssetPackage* package,
        const Guid&    guid,
        bool           loadTemp)
    {
        if (package->ContainsAsset(guid))
        {
            return package->LoadAsset(guid, loadTemp);
        }

        return {};
    }

    Co::result<Ptr<IAsset>> Manager::ReloadAsync(
        Guid guid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(guid, true))
            {
                break;
            }
        }

        return LoadAsync(guid);
    }

    Ptr<IAsset> Manager::Reload(
        const Guid& guid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(guid, true))
            {
                break;
            }
        }

        return Load(guid);
    }

    bool Manager::Unload(
        const Guid& guid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(guid, true))
            {
                return true;
            }
        }

        return false;
    }

    bool Manager::RequestUnload(
        const Guid& guid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(guid, false))
            {
                return true;
            }
        }

        return false;
    }
} // namespace Ame::Asset