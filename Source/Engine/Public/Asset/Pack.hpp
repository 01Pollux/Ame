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

    class IAssetPackage : public IObjectWithCallback
    {
        friend class Storage;
        friend class Manager;

    protected:
        using AssetCacheMap = std::unordered_map<UId, Ptr<IAsset>, UIdUtils::Hasher>;

        using RLock  = std::shared_lock<std::shared_mutex>;
        using RWLock = std::unique_lock<std::shared_mutex>;

    public:
        IMPLEMENT_INTERFACE_CTOR(IAssetPackage, Storage& assetStorage);

        IAssetPackage(const IAssetPackage&) = delete;
        IAssetPackage(IAssetPackage&&)      = delete;

        IAssetPackage& operator=(const IAssetPackage&) = delete;
        IAssetPackage& operator=(IAssetPackage&&)      = delete;

        virtual ~IAssetPackage() = default;

    public:
        /// <summary>
        /// Get the assets in this package as a coroutine.
        /// </summary>
        [[nodiscard]] virtual Co::generator<UId> GetAssets() = 0;

        /// <summary>
        /// Query if this package contains the given asset.
        /// </summary>
        [[nodiscard]] virtual bool ContainsAsset(const UId& uid) const = 0;

    public:
        /// <summary>
        /// Finds assets by path.
        /// </summary>
        [[nodiscard]] virtual UId FindAsset(const String& path) const = 0;

        /// <summary>
        /// Finds assets by path as regex.
        /// </summary>
        [[nodiscard]] virtual Co::generator<UId> FindAssets(const std::regex& pathRegex) const = 0;

    public:
        /// <summary>
        /// Export this package to the filesystem.
        /// </summary>
        virtual Co::result<void> Export() = 0;

        /// <summary>
        /// Add an asset to this package.
        /// </summary>
        virtual Co::result<void> SaveAsset(Ptr<IAsset> asset) = 0;

        /// <summary>
        /// Remove an asset from this package.
        /// </summary>
        virtual bool RemoveAsset(const UId& uid) = 0;

    protected:
        /// <summary>
        /// Load an asset from this package.
        /// </summary>
        [[nodiscard]] virtual Ptr<IAsset> LoadAsset(const UId& uid, bool loadTemp) = 0;

        /// <summary>
        /// Unload an asset from this package.
        /// </summary>
        virtual bool UnloadAsset(const UId& uid, bool force) = 0;

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