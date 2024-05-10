#pragma once

#include <Core/Enum.hpp>

#include <regex>
#include <vector>
#include <map>

#include <Core/Coroutine.hpp>
#include <Asset/Handle.hpp>
#include <Asset/Handler.hpp>
#include <Asset/Manager.hpp>

namespace Ame::Asset
{
    class IAsset;
    class IAssetPackage;
    class IAssetHandler;
    class Manager;

    enum class PackageFlags : uint8_t
    {
        /// <summary>
        /// The package is stored in memory.
        /// </summary>
        Memory = 1 << 0,

        /// <summary>
        /// The package is stored on disk.
        /// </summary>
        Disk = 1 << 1
    };

    struct AddDesc
    {
        /// <summary>
        /// The asset to add.
        /// </summary>
        Ptr<IAsset> Asset;

        /// <summary>
        /// The package to add the asset to.
        //  If null, the asset will be added to the first package.
        IAssetPackage* PreferredPackage = nullptr;

        /// <summary>
        /// The flags for the package.
        /// </summary>
        PackageFlags Flags = PackageFlags::Disk;
    };

    class Storage
    {
        using AssetPackageList = std::vector<UPtr<IAssetPackage>>;
        friend class IAssetPackage;

    public:
        Storage(
            Co::runtime& Runtime);

        Storage(const Storage&) = delete;
        Storage(Storage&&)      = default;

        Storage& operator=(const Storage&) = delete;
        Storage& operator=(Storage&&)      = default;

        ~Storage();

    public:
        /// <summary>
        /// Adds an asset to the storage system.
        /// </summary>
        Co::result<void> SaveAsset(
            const AddDesc& Desc);

        /// <summary>
        /// Removes an asset from the storage system.
        /// </summary>
        void RemoveAsset(
            const Handle& AssetGuid);

    public:
        /// <summary>
        /// Registers an asset handler.
        /// Not thread safe.
        /// </summary>
        void RegisterHandler(
            size_t              Id,
            UPtr<IAssetHandler> Handler);

        /// <summary>
        /// Registers an asset handler.
        /// Not thread safe.
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, IAsset>
        void RegisterHandler(
            ArgsTy&&... Args)
        {
            RegisterHandler(Ty::UID, std::make_unique<typename Ty::Handler>(std::forward<ArgsTy>(Args)...));
        }

        /// <summary>
        /// Unregisters an asset handler.
        /// Not thread safe.
        /// </summary>
        void UnregisterHandler(
            size_t Id);

        /// <summary>
        /// Gets the asset handler for the specified asset.
        /// Not thread safe.
        /// </summary>
        IAssetHandler* GetHandler(
            const Ptr<IAsset>& Asset,
            size_t*            Id = nullptr);

        /// <summary>
        /// Gets the asset handler for the specified id.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] IAssetHandler* GetHandler(
            size_t Id);

        /// <summary>
        /// Get the associated asset manager.
        /// </summary>
        [[nodiscard]] Manager& GetManager();

    private:
        /// <summary>
        /// Mounts an asset package.
        /// Not thread safe.
        /// </summary>
        IAssetPackage* Mount(
            UPtr<IAssetPackage> Package);

    public:
        /// <summary>
        /// Mounts an asset package.
        /// Not thread safe.
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, IAssetPackage>
        IAssetPackage* Mount(
            ArgsTy&&... Args)
        {
            return Mount(std::make_unique<Ty>(*this, std::forward<ArgsTy>(Args)...));
        }

        /// <summary>
        /// Unmounts an asset package.
        /// Not thread safe.
        /// </summary>
        void Unmount(
            IAssetPackage* Package);

        /// <summary>
        /// Helper function to exports all packages to disk.
        /// Not thread safe.
        /// </summary>
        void ExportAll();

    public:
        struct PackageAndAsset
        {
            IAssetPackage* Package;
            Handle         Guid;
        };

        /// <summary>
        /// Gets the asset package with the specified name.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] Co::generator<IAssetPackage*> GetPackages(
            const PackageFlags& Flags = PackageFlags::Disk);

        /// <summary>
        /// Gets all assets in all packages.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] Co::generator<PackageAndAsset> GetAllAssets(
            const PackageFlags& Flags = PackageFlags::Disk);

    public:
        /// <summary>
        /// Finds an asset by guid.
        /// </summary>
        [[nodiscard]] IAssetPackage* FindPackage(
            const Handle&       AssetGuid,
            const PackageFlags& Flags = PackageFlags::Disk);

        /// <summary>
        /// Finds assets by path.
        /// </summary>
        [[nodiscard]] PackageAndAsset FindAsset(
            const String&       Path,
            const PackageFlags& Flags = PackageFlags::Disk);

        /// <summary>
        /// Finds assets by path as regex.
        /// </summary>
        [[nodiscard]] Co::generator<PackageAndAsset> FindAssets(
            const std::regex&   PathRegex,
            const PackageFlags& Flags = PackageFlags::Disk);

    private:
        /// <summary>
        /// Used for initializing package and preventing mounts from multiple storages.
        /// </summary>
        [[nodiscard]] Co::runtime& GetRuntime() const;

    private:
        Ref<Co::runtime> m_Runtime;
        Manager          m_Manager;

        AssetPackageList                      m_Packages;
        std::map<size_t, UPtr<IAssetHandler>> m_Handlers;
    };

    using StorageUPtr = UPtr<Storage>;
} // namespace Ame::Asset