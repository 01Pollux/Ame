#pragma once

#include <Asset/Pack.hpp>

namespace Ame::Asset
{
    class MemoryAssetPackage : public IAssetPackage
    {
    public:
        using IAssetPackage::IAssetPackage;

    public:
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

    protected:
        Ptr<IAsset> LoadAsset(
            const Guid& guid,
            bool                 loadTemp) override;

        bool UnloadAsset(
            const Guid& guid,
            bool                 force) override;
    };
} // namespace Ame::Asset