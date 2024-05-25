#pragma once

#include <Asset/Pack.hpp>
#include <filesystem>

namespace Ame::Asset
{
    class DirectoryAssetPackage : public IAssetPackage
    {
        using AssetMetaMap = std::unordered_map<Guid, AssetMetaDataDef>;
        using AssetPathMap = std::unordered_map<String, Guid>;

    public:
        DirectoryAssetPackage(
            Storage&              storage,
            std::filesystem::path path);

        [[nodiscard]] Co::generator<Guid> GetAssets() override;

        bool ContainsAsset(
            const Guid& guid) const override;

    public:
        Guid FindAsset(
            const String& path) const override;

        Co::generator<Guid> FindAssets(
            const std::regex& pathRegex) const override;

    public:
        Co::result<void> Export() override;

        Co::result<void> SaveAsset(
            Ptr<IAsset> asset) override;

        bool RemoveAsset(
            const Guid& guid) override;

    public:
        /// <summary>
        /// Get asset's guid from a path, or returns an empty guid if not found.
        /// </summary>
        const Guid& GetGuidOfPath(
            const String& path) const;

    protected:
        Ptr<IAsset> LoadAsset(
            const Guid& guid,
            bool        loadTemp) override;

        bool UnloadAsset(
            const Guid& guid,
            bool        force) override;

    private:
        /// <summary>
        /// Get all files in the directory, recursively
        /// </summary>
        [[nodiscard]] static Co::generator<String> GetFiles(
            const std::filesystem::path& path);

        /// <summary>
        /// Export all asset metadata to file
        /// </summary>
        void ExportMeta(
            AssetMetaDataDef& meta) const;

        /// <summary>
        /// Load asset from the cache if it exists
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetFromCache(
            const Guid& guid);

        /// <summary>
        /// Load asset and its dependencies
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetAndDependencies(
            const Guid& guid,
            bool        loadTemp);

        /// <summary>
        /// Save asset and its dependencies
        /// </summary>
        void SaveAssetAndDependencies(
            Ptr<IAsset> asset);

    private:
        std::filesystem::path m_RootPath;
        AssetMetaMap          m_AssetMeta;
        AssetPathMap          m_AssetPath;
    };
} // namespace Ame::Asset