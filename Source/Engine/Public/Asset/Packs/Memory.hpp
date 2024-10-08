#pragma once

#include <Asset/Pack.hpp>

namespace Ame::Asset
{
    class MemoryAssetPackage : public IAssetPackage
    {
    public:
        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(IID_MemoryAssetPackage, IID_BaseAssetPackage, IAssetPackage);

    private:
        IMPLEMENT_INTERFACE_CTOR(MemoryAssetPackage, Storage& assetStorage) : IAssetPackage(counters, assetStorage)
        {
        }

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

    protected:
        Ptr<IAsset> LoadAsset(const UId& uid, bool loadTemp) override;

        bool UnloadAsset(const UId& uid, bool force) override;
    };
} // namespace Ame::Asset