#include <Asset/Storage.hpp>
#include <Asset/Manager.hpp>
#include <Asset/Pack.hpp>
#include <Asset/Exception.hpp>

#include <Log/Logger.hpp>

namespace Ame::Asset
{
    Co::result<Ptr<IAsset>> Manager::LoadAsync(
        IAssetPackage* Package,
        const Handle&  AssetGuid,
        bool           LoadTemp)
    {
        if (Package->ContainsAsset(AssetGuid)) [[likely]]
        {
            auto Executor = m_Runtime.get().background_executor();
            co_await Co::resume_on(Executor);
            co_return Package->LoadAsset(AssetGuid, LoadTemp);
        }

        co_return nullptr;
    }

    Co::result<Ptr<IAsset>> Manager::LoadAsync(
        const Handle& AssetGuid,
        bool          LoadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->ContainsAsset(AssetGuid))
            {
                auto Executor = m_Runtime.get().background_executor();
                co_await Co::resume_on(Executor);
                co_return Package->LoadAsset(AssetGuid, LoadTemp);
            }
        }

        co_return nullptr;
    }

    Ptr<IAsset> Manager::Load(
        const Handle& AssetGuid,
        bool          LoadTemp)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->ContainsAsset(AssetGuid))
            {
                return Package->LoadAsset(AssetGuid, LoadTemp);
            }
        }

        return nullptr;
    }

    Ptr<IAsset> Manager::Load(
        IAssetPackage* Package,
        const Handle&  AssetGuid,
        bool           LoadTemp)
    {
        if (Package->ContainsAsset(AssetGuid))
        {
            return Package->LoadAsset(AssetGuid, LoadTemp);
        }

        return nullptr;
    }

    Co::result<Ptr<IAsset>> Manager::ReloadAsync(
        const Handle& AssetGuid)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->UnloadAsset(AssetGuid, true))
            {
                break;
            }
        }
        return LoadAsync(AssetGuid);
    }

    Ptr<IAsset> Manager::Reload(
        const Handle& AssetGuid)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->UnloadAsset(AssetGuid, true))
            {
                break;
            }
        }
        return Load(AssetGuid);
    }

    bool Manager::Unload(
        const Handle& AssetGuid)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->UnloadAsset(AssetGuid, true))
            {
                return true;
            }
        }

        return false;
    }

    bool Manager::RequestUnload(
        const Handle& AssetGuid)
    {
        using namespace EnumBitOperators;

        PackageFlags Flags = PackageFlags::Disk | PackageFlags::Memory;
        for (auto Package : m_Storage.get().GetPackages(Flags))
        {
            if (Package->UnloadAsset(AssetGuid, false))
            {
                return true;
            }
        }

        return false;
    }
} // namespace Ame::Asset