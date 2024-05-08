#pragma once

#include <Asset/Storage.hpp>
#include <Asset/Manager.hpp>

#include <mutex>
#include <vector>
#include <map>

namespace Ame::Asset
{
    class DefaultStorage : public IStorage
    {
        using AssetPackageList = std::vector<UPtr<IAssetPackage>>;

    public:
        DefaultStorage(
            Co::runtime& Runtime);
        ~DefaultStorage() override;

    public:
        Co::result<void> SaveAsset(
            const AddDesc& Desc) override;

        void RemoveAsset(
            const Handle& AssetGuid) override;

    public:
        void RegisterHandler(
            size_t              Id,
            UPtr<IAssetHandler> Handler) override;

        void UnregisterHandler(
            size_t Id) override;

        IAssetHandler* GetHandler(
            const Ptr<IAsset>& Asset,
            size_t*            Id) override;

        IAssetHandler* GetHandler(
            size_t Id) override;

        Manager& GetManager() override;

    protected:
        IAssetPackage* Mount(
            UPtr<IAssetPackage> Package) override;

    public:
        void Unmount(
            IAssetPackage* Package) override;

    public:
        [[nodiscard]] Co::generator<IAssetPackage*> GetPackages(
            const PackageFlags& Flags) override;

        [[nodiscard]] Co::generator<PackageAndAsset> GetAllAssets(
            const PackageFlags& Flags) override;

    private:
        AssetPackageList                      m_Packages;
        std::map<size_t, UPtr<IAssetHandler>> m_Handlers;
        Manager                               m_Manager;
    };
} // namespace Ame::Asset