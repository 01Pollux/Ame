#include <Asset/Storage.hpp>
#include <Asset/Manager.hpp>
#include <Asset/Pack.hpp>
#include <Asset/Exception.hpp>

#include <Log/Logger.hpp>

namespace Ame::Asset
{
    Co::result<Ptr<IAsset>> Manager::LoadAsync(IAssetPackage* package, UId uid, bool loadTemp)
    {
        if (package->ContainsAsset(uid)) [[likely]]
        {
            co_return package->LoadAsset(uid, loadTemp);
        }

        co_return nullptr;
    }

    Co::result<Ptr<IAsset>> Manager::LoadAsync(UId uid, bool loadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->ContainsAsset(uid))
            {
                co_return package->LoadAsset(uid, loadTemp);
            }
        }

        co_return nullptr;
    }

    Ptr<IAsset> Manager::Load(const UId& uid, bool loadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->ContainsAsset(uid))
            {
                return package->LoadAsset(uid, loadTemp);
            }
        }

        return {};
    }

    Ptr<IAsset> Manager::Load(IAssetPackage* package, const UId& uid, bool loadTemp)
    {
        if (package->ContainsAsset(uid))
        {
            return package->LoadAsset(uid, loadTemp);
        }

        return {};
    }

    Co::result<Ptr<IAsset>> Manager::ReloadAsync(UId uid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(uid, true))
            {
                break;
            }
        }

        return LoadAsync(uid);
    }

    Ptr<IAsset> Manager::Reload(const UId& uid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(uid, true))
            {
                break;
            }
        }

        return Load(uid);
    }

    bool Manager::Unload(const UId& uid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(uid, true))
            {
                return true;
            }
        }

        return false;
    }

    bool Manager::RequestUnload(const UId& uid)
    {
        using namespace EnumBitOperators;

        PackageFlags flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto package : m_Storage.get().GetPackages(flags))
        {
            if (package->UnloadAsset(uid, false))
            {
                return true;
            }
        }

        return false;
    }
} // namespace Ame::Asset