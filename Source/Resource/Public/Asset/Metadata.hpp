#pragma once

#include <span>
#include <boost/property_tree/ptree.hpp>
#include <Asset/Core.hpp>

namespace Ame::Asset
{
    using AssetMetaData = boost::property_tree::ptree;

    struct AssetMetaDataDef
    {
        static constexpr const char c_MetaFileExtension[]     = ".aamd"; // Ame Asset Meta Data
        static constexpr size_t     c_MetaFileExtensionLength = std::size(c_MetaFileExtension) - 1;

        /// <summary>
        /// Creating an asset's metadata from an input stream.
        /// </summary>
        explicit AssetMetaDataDef(
            std::istream& stream);

        /// <summary>
        /// Creating an empty asset's metadata.
        /// </summary>
        AssetMetaDataDef(
            const Guid& guid,
            String      path);

        /// <summary>
        /// Creating an asset's metadata from an input stream.
        /// </summary>
        void Export(
            std::ostream& stream) const;

        /// <summary>
        /// Get the asset's GUID.
        /// </summary>
        [[nodiscard]] Guid GetGuid() const noexcept;

        /// <summary>
        /// Set the asset's GUID.
        /// </summary>
        void SetGuid(
            const Guid& guid) noexcept;

        /// <summary>
        /// Get the asset's hash.
        /// </summary>
        [[nodiscard]] String GetHash() const noexcept;

        /// <summary>
        /// Set the asset's hash.
        /// </summary>
        void SetHash(
            String hash) noexcept;

        /// <summary>
        /// Get the asset's loader id.
        /// </summary>
        [[nodiscard]] size_t GetLoaderId() const noexcept;

        /// <summary>
        /// Set the asset's loader id.
        /// </summary>
        void SetLoaderId(
            size_t id) noexcept;

        /// <summary>
        /// Get the asset's loader data.
        /// </summary>
        [[nodiscard]] AssetMetaData& GetLoaderData() noexcept;

        /// <summary>
        /// Set the asset's loader data.
        /// </summary>
        [[nodiscard]] const AssetMetaData& GetLoaderData() const noexcept;

        /// <summary>
        /// Get the asset's path.
        /// </summary>
        [[nodiscard]] std::filesystem::path GetAssetPath() const;

        /// <summary>
        /// Get the asset's metadata path.
        /// </summary>
        [[nodiscard]] std::filesystem::path GetMetaPath() const;

        /// <summary>
        /// Set the asset's metadata path.
        /// </summary>
        void SetMetaPath(
            String path);

        /// <summary>
        /// Query if the asset is dirty.
        /// </summary>
        [[nodiscard]] bool IsDirty() const noexcept;

        /// <summary>
        /// Marks the asset as dirty.
        /// </summary>
        void SetDirty(
            bool isDirty = true) noexcept;

        /// <summary>
        /// Get the asset's dependencies.
        /// </summary>
        [[nodiscard]] Co::generator<Guid> GetDependencies() const;

        /// <summary>
        /// Set the asset's dependencies.
        /// </summary>
        void SetDependencies(
            std::span<String> dependencies);

    private:
        AssetMetaData m_MetaData;
        bool          m_IsDirty = false;
    };
} // namespace Ame::Asset