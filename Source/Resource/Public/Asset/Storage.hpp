#pragma once

#include <Core/Enum.hpp>

#include <regex>
#include <vector>
#include <map>

#include <Core/Coroutine.hpp>
#include <Asset/Core.hpp>
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
        using AssetPackageList = std::vector<Ptr<IAssetPackage>>;
        using AssetHandlerMap  = std::map<size_t, Ptr<IAssetHandler>>;

        friend class IAssetPackage;

    public:
        Storage(
            SharedPtr<Co::runtime> coroutine);

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
            const AddDesc& desc);

        /// <summary>
        /// Removes an asset from the storage system.
        /// </summary>
        void RemoveAsset(
            const Guid& guid);

    public:
        /// <summary>
        /// Registers an asset handler.
        /// Not thread safe.
        /// </summary>
        void RegisterHandler(
            size_t             id,
            Ptr<IAssetHandler> handler);

        /// <summary>
        /// Registers an asset handler.
        /// Not thread safe.
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, IAssetHandler>
        void RegisterHandler(
            ArgsTy&&... args)
        {
            RegisterHandler(Ty::UID, std::make_unique<Ty>(std::forward<ArgsTy>(args)...));
        }

        /// <summary>
        /// Unregisters an asset handler.
        /// Not thread safe.
        /// </summary>
        void UnregisterHandler(
            size_t id);

        /// <summary>
        /// Gets the asset handler for the specified asset.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] std::pair<IAssetHandler*, size_t> GetHandler(
            const Ptr<IAsset>& asset);

        /// <summary>
        /// Gets the asset handler for the specified id.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] IAssetHandler* GetHandler(
            size_t id);

        /// <summary>
        /// Get the associated asset manager.
        /// </summary>
        [[nodiscard]] Manager& GetManager();

    private:
        /// <summary>
        /// Mounts an asset package.
        /// Not thread safe.
        /// </summary>
        void Mount(
            Ptr<IAssetPackage> package);

    public:
        /// <summary>
        /// Mounts an asset package.
        /// Not thread safe.
        /// </summary>
        template<typename Ty, typename... ArgsTy>
            requires std::derived_from<Ty, IAssetPackage>
        IAssetPackage* Mount(
            ArgsTy&&... args)
        {
            return Mount(std::make_unique<Ty>(*this, std::forward<ArgsTy>(args)...));
        }

        /// <summary>
        /// Unmounts an asset package.
        /// Not thread safe.
        /// </summary>
        void Unmount(
            IAssetPackage* package);

        /// <summary>
        /// Helper function to exports all packages to disk.
        /// Not thread safe.
        /// </summary>
        void ExportAll();

    public:
        struct PackageAndAsset
        {
            Ptr<IAssetPackage> Package;
            Guid               Handle;
        };

        /// <summary>
        /// Gets the asset package with the specified name.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] Co::generator<Ptr<IAssetPackage>> GetPackages(
            const PackageFlags& flags = PackageFlags::Disk);

        /// <summary>
        /// Gets all assets in all packages.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] Co::generator<PackageAndAsset> GetAllAssets(
            const PackageFlags& flags = PackageFlags::Disk);

    public:
        /// <summary>
        /// Finds an asset by guid.
        /// </summary>
        [[nodiscard]] IAssetPackage* FindPackage(
            const Guid&         guid,
            const PackageFlags& flags = PackageFlags::Disk);

        /// <summary>
        /// Finds assets by path.
        /// </summary>
        [[nodiscard]] PackageAndAsset FindAsset(
            const String&       path,
            const PackageFlags& flags = PackageFlags::Disk);

        /// <summary>
        /// Finds assets by path as regex.
        /// </summary>
        [[nodiscard]] Co::generator<PackageAndAsset> FindAssets(
            const std::regex&   pathRegex,
            const PackageFlags& flags = PackageFlags::Disk);

    private:
        /// <summary>
        /// Used for initializing package and preventing mounts from multiple storages.
        /// </summary>
        [[nodiscard]] Co::runtime& GetRuntime() const;

    private:
        SharedPtr<Co::runtime> m_Runtime;
        Manager                m_Manager;

        AssetPackageList m_Packages;
        AssetHandlerMap  m_Handlers;
    };
} // namespace Ame::Asset