#pragma once

#include <unordered_map>
#include <shared_mutex>
#include <future>
#include <regex>
#include <shared_mutex>

#include <Asset/Asset.hpp>
#include <Asset/Metadata.hpp>

namespace Ame::Asset
{
    class Storage;
    class Manager;

    class IAssetPackage
    {
        friend class Storage;
        friend class Manager;

    protected:
        using AssetCacheMap = std::unordered_map<Guid, Ptr<IAsset>>;

        using RLock  = std::shared_lock<std::shared_mutex>;
        using RWLock = std::unique_lock<std::shared_mutex>;

    public:
        IAssetPackage(
            Storage& assetStorage);

        IAssetPackage(const IAssetPackage&) = delete;
        IAssetPackage(IAssetPackage&&)      = delete;

        IAssetPackage& operator=(const IAssetPackage&) = delete;
        IAssetPackage& operator=(IAssetPackage&&)      = delete;

        virtual ~IAssetPackage() = default;

    public:
        /// <summary>
        /// Get the assets in this package as a coroutine.
        /// </summary>
        [[nodiscard]] virtual Co::generator<Guid> GetAssets() = 0;

        /// <summary>
        /// Query if this package contains the given asset.
        /// </summary>
        [[nodiscard]] virtual bool ContainsAsset(
            const Guid& guid) const = 0;

    public:
        /// <summary>
        /// Finds assets by path.
        /// </summary>
        [[nodiscard]] virtual Guid FindAsset(
            const String& path) const = 0;

        /// <summary>
        /// Finds assets by path as regex.
        /// </summary>
        [[nodiscard]] virtual Co::generator<Guid> FindAssets(
            const std::regex& pathRegex) const = 0;

    public:
        /// <summary>
        /// Export this package to the filesystem.
        /// </summary>
        virtual Co::result<void> Export() = 0;

        /// <summary>
        /// Add an asset to this package.
        /// </summary>
        virtual Co::result<void> SaveAsset(
            Ptr<IAsset> asset) = 0;

        /// <summary>
        /// Remove an asset from this package.
        /// </summary>
        virtual bool RemoveAsset(
            const Guid& guid) = 0;

    protected:
        /// <summary>
        /// Load an asset from this package.
        /// </summary>
        [[nodiscard]] virtual Ptr<IAsset> LoadAsset(
            const Guid& guid,
            bool        loadTemp) = 0;

        /// <summary>
        /// Unload an asset from this package.
        /// </summary>
        virtual bool UnloadAsset(
            const Guid& guid,
            bool        force) = 0;

    private:
        /// <summary>
        /// Used for sanity checks against mounting to multiple storages.
        /// </summary>
        [[nodiscard]] Storage& GetStorage() const;

    protected:
        Ref<Storage>     m_Storage;
        Ref<Co::runtime> m_Runtime;

        AssetCacheMap             m_Cache;
        mutable std::shared_mutex m_CacheMutex;
    };
} // namespace Ame::Asset