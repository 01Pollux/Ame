#pragma once

#include <Asset/Handle.hpp>
#include <boost/property_tree/ptree.hpp>
#include <FileSystem/Path.hpp>
#include <span>

namespace Ame::Asset
{
    using AssetMetaData = boost::property_tree::ptree;

    struct AssetMetaDataDef
    {
        static constexpr const char s_MetaFileExtension[]     = ".aamd"; // Ame Asset Meta Data
        static constexpr size_t     s_MetaFileExtensionLength = std::size(s_MetaFileExtension) - 1;

        /// <summary>
        /// Creating an asset's metadata from an input stream.
        /// </summary>
        explicit AssetMetaDataDef(
            std::istream& Stream);

        /// <summary>
        /// Creating an empty asset's metadata.
        /// </summary>
        AssetMetaDataDef(
            const Handle& AssetGuid,
            String        Path);

        /// <summary>
        /// Creating an asset's metadata from an input stream.
        /// </summary>
        void Export(
            std::ostream& Stream) const;

        /// <summary>
        /// Get the asset's GUID.
        /// </summary>
        [[nodiscard]] Handle GetGuid() const noexcept;

        /// <summary>
        /// Set the asset's GUID.
        /// </summary>
        void SetGuid(
            const Handle& Guid) noexcept;

        /// <summary>
        /// Get the asset's hash.
        /// </summary>
        [[nodiscard]] String GetHash() const noexcept;

        /// <summary>
        /// Set the asset's hash.
        /// </summary>
        void SetHash(
            String Hash) noexcept;

        /// <summary>
        /// Get the asset's loader id.
        /// </summary>
        [[nodiscard]] size_t GetLoaderId() const noexcept;

        /// <summary>
        /// Set the asset's loader id.
        /// </summary>
        void SetLoaderId(
            size_t Id) noexcept;

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
            String Path);

        /// <summary>
        /// Query if the asset is dirty.
        /// </summary>
        [[nodiscard]] bool IsDirty() const noexcept;

        /// <summary>
        /// Marks the asset as dirty.
        /// </summary>
        void SetDirty(
            bool IsDirty = true) noexcept;

        /// <summary>
        /// Get the asset's dependencies.
        /// </summary>
        [[nodiscard]] Co::generator<Handle> GetDependencies() const;

        /// <summary>
        /// Set the asset's dependencies.
        /// </summary>
        void SetDependencies(
            std::span<String> Dependencies);

    private:
        AssetMetaData m_MetaData;
        bool          m_IsDirty = false;
    };
} // namespace Ame::Asset