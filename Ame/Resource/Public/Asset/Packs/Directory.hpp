#pragma once

#include <Asset/Pack.hpp>
#include <filesystem>

namespace Ame::Asset
{
    class DirectoryAssetPackage : public IAssetPackage
    {
        using AssetMetaMap = std::unordered_map<Handle, AssetMetaDataDef>;
        using AssetPathMap = std::unordered_map<String, Handle>;

    public:
        DirectoryAssetPackage(
            IStorage&             Storage,
            Co::runtime&          Runtime,
            std::filesystem::path Path);

        [[nodiscard]] Co::generator<const Asset::Handle&> GetAssets() override;

        bool ContainsAsset(
            const Asset::Handle& AssetGuid) const override;

    public:
        Asset::Handle FindAsset(
            const String& Path) const override;

        Co::generator<const Asset::Handle&> FindAssets(
            const std::regex& PathRegex) const override;

    public:
        Co::result<void> Export() override;

        Co::result<void> SaveAsset(
            Ptr<IAsset> Asset) override;

        bool RemoveAsset(
            const Asset::Handle& AssetGuid) override;

    public:
        /// <summary>
        /// Get asset's guid from a path, or returns an empty guid if not found.
        /// </summary>
        const Asset::Handle& GetGuidOfPath(
            const String& Path) const;

    protected:
        Ptr<IAsset> LoadAsset(
            const Asset::Handle& AssetGuid,
            bool                 LoadTemp) override;

        bool UnloadAsset(
            const Asset::Handle& AssetGuid,
            bool                 Force) override;

    private:
        /// <summary>
        /// Get all files in the directory, recursively
        /// </summary>
        [[nodiscard]] static Co::generator<String> GetFiles(
            const std::filesystem::path& Path);

        /// <summary>
        /// Export all asset metadata to file
        /// </summary>
        void ExportMeta(
            AssetMetaDataDef& Meta) const;

        /// <summary>
        /// Load asset from the cache if it exists
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetFromCache(
            const Asset::Handle& AssetGuid);

        /// <summary>
        /// Load asset and its dependencies
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetAndDependencies(
            const Asset::Handle& AssetGuid,
            bool                 LoadTemp);

        /// <summary>
        /// Save asset and its dependencies
        /// </summary>
        void SaveAssetAndDependencies(
            Ptr<IAsset> FirstAsset);

    private:
        std::filesystem::path m_RootPath;
        AssetMetaMap          m_AssetMeta;
        AssetPathMap          m_AssetPath;
    };
} // namespace Ame::Asset