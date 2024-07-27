#pragma once

#include <Asset/Asset.hpp>
#include <Asset/Metadata.hpp>

#include <boost/serialization/vector.hpp>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <queue>

namespace Ame::Asset
{
    class DependencyReader;
    class DependencyWriter;

    //

    class DependencyReader
    {
    public:
        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename Ty, typename ArchiveTy>
            requires std::is_base_of_v<IAsset, Ty>
        [[nodiscard]] Ptr<Ty> ReadOne(
            ArchiveTy& archive) const
        {
            Guid guid;
            archive >> guid;
            if (guid != Guid::c_Null)
            {
                auto iter = m_Assets.find(guid);
                if (iter != m_Assets.end())
                {
                    return std::dynamic_pointer_cast<Ty>(iter->second);
                }
            }
            return nullptr;
        }

        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename Ty = IAsset, typename ArchiveTy>
            requires std::is_base_of_v<IAsset, Ty>
        [[nodiscard]] auto ReadMany(
            ArchiveTy& archive) const
        {
            std::vector<Guid> guids;
            archive >> guids;

            std::vector<Ptr<Ty>> assets;
            assets.reserve(guids.size());

            for (const auto& guid : guids)
            {
                Ptr<Ty> asset;
                if (guid != Guid::c_Null)
                {
                    auto iter = m_Assets.find(guid);
                    if (iter != m_Assets.end())
                    {
                        asset = std::dynamic_pointer_cast<Ty>(iter->second);
                    }
                }
                if (asset)
                {
                    assets.emplace_back(std::move(asset));
                }
            }

            return assets;
        }

    public:
        /// <summary>
        /// Internal use only for package reading.
        /// Link an asset for depdendency reading.
        /// </summary>
        void Link(
            const Guid&        guid,
            const Ptr<IAsset>& asset)
        {
            m_Assets.emplace(guid, asset);
        }

    private:
        std::unordered_map<Guid, Ptr<IAsset>> m_Assets;
    };

    //

    class DependencyWriter
    {
    public:
        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename ArchiveTy, typename Ty>
            requires std::is_base_of_v<IAsset, Ty>
        void WriteOne(
            ArchiveTy&     archive,
            const Ptr<Ty>& asset)
        {
            if (asset)
            {
                archive << asset->GetGuid();
                m_Assets.emplace(asset);
            }
            else
            {
                archive << Guid::c_Null;
            }
        }

        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename ArchiveTy, typename Ty>
        void WriteMany(
            ArchiveTy& archive,
            const Ty&  assets)
        {
            std::vector<Guid> handles;
            for (auto& asset : assets)
            {
                m_Assets.emplace(asset);
                handles.emplace_back(asset->GetGuid());
            }
            archive << handles;
        }

    public:
        /// <summary>
        /// Internal use only for package writing.
        /// Get the dependencies of an asset.
        /// </summary>
        auto& GetDependencies() noexcept
        {
            return m_Assets;
        }

    private:
        std::unordered_set<Ptr<IAsset>> m_Assets;
    };

    //

    struct AssetHandlerLoadDesc
    {
        SharedPtr<Co::executor> BackgroundExecutor;
        SharedPtr<Co::executor> ForegroundExecutor;

        Ref<std::istream>      Stream;
        CRef<DependencyReader> Dependencies;

        Guid   Guid;
        String Path;

        CRef<AssetMetaData> LoaderData;
    };

    struct AssetHandlerSaveDesc
    {
        SharedPtr<Co::executor> BackgroundExecutor;
        SharedPtr<Co::executor> ForegroundExecutor;

        Ref<std::iostream>    Stream;
        Ref<DependencyWriter> Dependencies;

        Ptr<IAsset>        Asset;
        Ref<AssetMetaData> LoaderData;
    };

    //

    class IAssetHandler : public IObject
    {
    public:
        /// <summary>
        /// Query if this asset handler can handle the given asset.
        /// </summary>
        virtual bool CanHandle(
            const Ptr<IAsset>& asset) = 0;

        /// <summary>
        /// Load the asset from an input stream.
        /// </summary>
        [[nodiscard]] virtual Co::result<Ptr<IAsset>> Load(
            AssetHandlerLoadDesc& desc) = 0;

        /// <summary>
        /// Save the asset to an output stream.
        /// </summary>
        [[nodiscard]] virtual Co::result<void> Save(
            AssetHandlerSaveDesc& desc) = 0;
    };

    //

#define AME_STANDARD_ASSET_HANDLER_BODY           \
    bool CanHandle(                               \
        const Ptr<IAsset>& asset) override;       \
                                                  \
    Co::result<Ptr<IAsset>> Load(                 \
        AssetHandlerLoadDesc& loadDesc) override; \
                                                  \
    Co::result<void> Save(                        \
        AssetHandlerSaveDesc& saveDesc) override

#define AME_STANDARD_ASSET_HANDLER(Name, ID) \
    class Name : public IAssetHandler        \
    {                                        \
    public:                                  \
        static constexpr size_t UID = ID;    \
                                             \
    public:                                  \
        AME_STANDARD_ASSET_HANDLER_BODY;     \
    };

    //

    /// <summary>
    /// By default, the asset handler will call IAsset::Serialize and IAsset::Deserialize.
    /// </summary>
    AME_STANDARD_ASSET_HANDLER(DefaultAssetHandler, 0xFFFF'FFFF'FFFF'FFFF);
} // namespace Ame::Asset