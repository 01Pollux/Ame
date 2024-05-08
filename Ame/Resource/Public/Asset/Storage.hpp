#pragma once

#include <Core/Enum.hpp>

#include <Core/Coroutine.hpp>
#include <Asset/Handle.hpp>
#include <Asset/Handler.hpp>

#include <regex>

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

    class IStorage
    {
    public:
        using IocParams = Ptr<Co::runtime>;

        IStorage(
            Co::runtime& Runtime) :
            m_Runtime(Runtime)
        {
        }

        IStorage(const IStorage&) = delete;
        IStorage(IStorage&&)      = delete;

        IStorage& operator=(const IStorage&) = delete;
        IStorage& operator=(IStorage&&)      = delete;

        virtual ~IStorage() = default;

        /// <summary>
        /// Adds an asset to the storage system.
        /// </summary>
        virtual Co::result<void> SaveAsset(
            const AddDesc& Desc) = 0;

        /// <summary>
        /// Removes an asset from the storage system.
        /// </summary>
        virtual void RemoveAsset(
            const Handle& AssetGuid) = 0;

    public:
        /// <summary>
        /// Registers an asset handler.
        /// Not thread safe.
        /// </summary>
        virtual void RegisterHandler(
            size_t              Id,
            UPtr<IAssetHandler> Handler) = 0;

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
        virtual void UnregisterHandler(
            size_t Id) = 0;

        /// <summary>
        /// Gets the asset handler for the specified asset.
        /// Not thread safe.
        /// </summary>
        virtual IAssetHandler* GetHandler(
            const Ptr<IAsset>& Asset,
            size_t*            Id = nullptr) = 0;

        /// <summary>
        /// Gets the asset handler for the specified id.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] virtual IAssetHandler* GetHandler(
            size_t Id) = 0;

        /// <summary>
        /// Get the associated asset manager.
        /// </summary>
        [[nodiscard]] virtual Manager& GetManager() = 0;

    protected:
        /// <summary>
        /// Mounts an asset package.
        /// Not thread safe.
        /// </summary>
        virtual IAssetPackage* Mount(
            UPtr<IAssetPackage> Package) = 0;

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
            return Mount(std::make_unique<Ty>(*this, m_Runtime, std::forward<Args>(Args)...));
        }

        /// <summary>
        /// Unmounts an asset package.
        /// Not thread safe.
        /// </summary>
        virtual void Unmount(
            IAssetPackage* Package) = 0;

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
        [[nodiscard]] virtual Co::generator<IAssetPackage*> GetPackages(
            const PackageFlags& Flags = PackageFlags::Disk) = 0;

        /// <summary>
        /// Gets all assets in all packages.
        /// Not thread safe.
        /// </summary>
        [[nodiscard]] virtual Co::generator<PackageAndAsset> GetAllAssets(
            const PackageFlags& Flags = PackageFlags::Disk) = 0;

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

    protected:
        Ref<Co::runtime> m_Runtime;
    };

    using StorageUPtr = UPtr<IStorage>;
} // namespace Ame::Asset