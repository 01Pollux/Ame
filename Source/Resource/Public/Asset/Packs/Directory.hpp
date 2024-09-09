#pragma once

#include <Asset/Pack.hpp>
#include <filesystem>

namespace Ame::Asset
{
    class DirectoryAssetPackage : public BaseObject<IAssetPackage>
    {
    public:
        using Base = BaseObject<IAssetPackage>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(IID_DirectoryAssetPackage, IID_BaseAssetPackage, Base);

    public:
        using AssetMetaMap = std::unordered_map<UId, AssetMetaDataDef, UIdUtils::Hasher>;
        using AssetPathMap = std::unordered_map<String, UId>;

    private:
        IMPLEMENT_INTERFACE_CTOR(DirectoryAssetPackage, Storage& storage, std::filesystem::path path);

    public:
        [[nodiscard]] Co::generator<UId> GetAssets() override;

        bool ContainsAsset(const UId& uid) const override;

    public:
        UId FindAsset(const String& path) const override;

        Co::generator<UId> FindAssets(const std::regex& pathRegex) const override;

    public:
        Co::result<void> Export() override;

        Co::result<void> SaveAsset(Ptr<IAsset> asset) override;

        bool RemoveAsset(const UId& uid) override;

    public:
        /// <summary>
        /// Get asset's uid from a path, or returns an empty uid if not found.
        /// </summary>
        const UId& GetGuidOfPath(const String& path) const;

    protected:
        Ptr<IAsset> LoadAsset(const UId& uid, bool loadTemp) override;

        bool UnloadAsset(const UId& uid, bool force) override;

    private:
        /// <summary>
        /// Get all files in the directory, recursively
        /// </summary>
        [[nodiscard]] static Co::generator<String> GetFiles(const std::filesystem::path& path);

        /// <summary>
        /// Export all asset metadata to file
        /// </summary>
        void ExportMeta(AssetMetaDataDef& meta) const;

        /// <summary>
        /// Load asset from the cache if it exists
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetFromCache(const UId& uid);

        /// <summary>
        /// Load asset and its dependencies
        /// </summary>
        [[nodiscard]] Ptr<IAsset> LoadAssetAndDependencies(const UId& uid, bool loadTemp);

        /// <summary>
        /// Save asset and its dependencies
        /// </summary>
        void SaveAssetAndDependencies(Ptr<IAsset> asset);

    private:
        std::filesystem::path m_RootPath;
        AssetMetaMap          m_AssetMeta;
        AssetPathMap          m_AssetPath;
    };
} // namespace Ame::Asset