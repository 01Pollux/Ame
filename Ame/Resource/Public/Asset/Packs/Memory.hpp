#pragma once

#include <Asset/Pack.hpp>

namespace Ame::Asset
{
    class MemoryAssetPackage : public IAssetPackage
    {
    public:
        using IAssetPackage::IAssetPackage;

    public:
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

    protected:
        Ptr<IAsset> LoadAsset(
            const Asset::Handle& AssetGuid,
            bool                 LoadTemp) override;

        bool UnloadAsset(
            const Asset::Handle& AssetGuid,
            bool                 Force) override;
    };
} // namespace Ame::Asset