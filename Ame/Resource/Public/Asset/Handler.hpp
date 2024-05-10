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

    class IAssetHandler
    {
    public:
        /// <summary>
        /// Query if this asset handler can handle the given asset.
        /// </summary>
        virtual bool CanHandle(
            const Ptr<IAsset>& Asset) = 0;

        /// <summary>
        /// Load the asset from an input stream.
        /// </summary>
        virtual Ptr<IAsset> Load(
            std::istream&                  Stream,
            const Asset::DependencyReader& DepReader,
            const Handle&                  AssetGuid,
            String                         Path,
            const AssetMetaData&           LoaderData) = 0;

        /// <summary>
        /// Save the asset to an output stream.
        /// </summary>
        virtual void Save(
            std::iostream&     Stream,
            DependencyWriter&  DepWriter,
            const Ptr<IAsset>& Asset,
            AssetMetaData&     LoaderData) = 0;
    };

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
            ArchiveTy& Archive) const
        {
            Handle AssetGuid;
            Archive >> AssetGuid;
            if (AssetGuid != Handle::Null)
            {
                auto Iter = m_Assets.find(AssetGuid);
                if (Iter != m_Assets.end())
                {
                    return std::dynamic_pointer_cast<Ty>(Iter->second);
                }
            }
            return nullptr;
        }

        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename Ty, typename ArchiveTy>
            requires std::is_base_of_v<IAsset, Ty>
        [[nodiscard]] auto ReadMany(
            ArchiveTy& Archive) const
        {
            std::vector<Handle> ChildGuids;
            Archive >> ChildGuids;

            std::vector<Ptr<Ty>> ChildAssets;
            ChildAssets.reserve(ChildGuids.size());

            for (const auto& ChildGuid : ChildGuids)
            {
                Ptr<Ty> Asset;
                if (ChildGuid != Handle::Null)
                {
                    auto AssetIter = m_Assets.find(ChildGuid);
                    if (AssetIter != m_Assets.end())
                    {
                        Asset = std::dynamic_pointer_cast<Ty>(AssetIter->second);
                    }
                }
                ChildAssets.emplace_back(std::move(Asset));
            }

            return ChildAssets;
        }

    public:
        /// <summary>
        /// Internal use only.
        /// Link an asset for depdendency reading.
        /// </summary>
        void Link(
            const Handle&      AssetGuid,
            const Ptr<IAsset>& Asset)
        {
            m_Assets.emplace(AssetGuid, Asset);
        }

    private:
        std::unordered_map<Handle, Ptr<IAsset>> m_Assets;
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
            ArchiveTy&     Archive,
            const Ptr<Ty>& Asset)
        {
            if (Asset)
            {
                Archive << Asset->GetGuid();
                m_Assets.emplace(Asset);
            }
            else
            {
                Archive << Handle::Null;
            }
        }

        /// <summary>
        /// Read the dependencies of an asset.
        /// </summary>
        template<typename ArchiveTy, typename Ty>
        void WriteMany(
            ArchiveTy& Archive,
            const Ty&  Assets)
        {
            std::vector<Handle> Handles;
            for (auto& Asset : Assets)
            {
                m_Assets.emplace(Asset);
                Handles.emplace_back(Asset->GetGuid());
            }
            Archive << Handles;
        }

    public:
        /// <summary>
        /// Internal use only.
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

#define AME_STANDARD_ASSET_HANDLER_BODY            \
    bool CanHandle(                                \
        const Ptr<IAsset>& Asset) override;        \
                                                   \
    Ptr<IAsset> Load(                              \
        std::istream&                  Stream,     \
        const Asset::DependencyReader& DepReader,  \
        const Handle&                  AssetGuid,  \
        String                         Path,       \
        const AssetMetaData&           LoaderData) override; \
                                                   \
    void Save(                                     \
        std::iostream&     Stream,                 \
        DependencyWriter&  DepWriter,              \
        const Ptr<IAsset>& Asset,                  \
        AssetMetaData&     LoaderData) override

#define AME_STANDARD_ASSET_HANDLER(Name) \
    class Name : public IAssetHandler    \
    {                                    \
    public:                              \
        AME_STANDARD_ASSET_HANDLER_BODY; \
    };
} // namespace Ame::Asset