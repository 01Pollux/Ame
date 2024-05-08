#include <Asset/Storages/DefaultStorage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Handler.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    DefaultStorage::DefaultStorage(
        Co::runtime& Runtime) :
        IStorage(Runtime),
        m_Manager(*this, Runtime)
    {
        auto Package = std::make_unique<MemoryAssetPackage>(*this, m_Runtime);
        m_Packages.emplace_back(std::move(Package));
    }

    DefaultStorage::~DefaultStorage() = default;

    //

    Co::result<void> DefaultStorage::SaveAsset(
        const AddDesc& Desc)
    {
        AME_LOG_ASSERT(Log::Asset(), Desc.Asset != nullptr, "Asset is null");

        using namespace EnumBitOperators;

        IAssetPackage* Package = Desc.PreferredPackage;
        if ((Desc.Flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            Package = m_Packages.front().get();
        }
        else if (!Package)
        {
            AME_LOG_ASSERT(Log::Asset(), m_Packages.size() > 1, "No packages mounted");
            Package = std::next(m_Packages.begin())->get();
        }
#if AME_DEBUG
        else
        {
            if (std::ranges::find_if(
                    m_Packages, [Package](const auto& CurPackage)
                    { return CurPackage.get() == Package; }) == m_Packages.end())
            {
                AME_LOG_ASSERT(Log::Asset(), false, "Package not mounted");
            }
        }
#endif

        return Package->SaveAsset(Desc.Asset);
    }

    void DefaultStorage::RemoveAsset(
        const Handle& AssetGuid)
    {
        for (auto& Package : m_Packages)
        {
            if (Package->RemoveAsset(AssetGuid))
            {
                return;
            }
        }

        Log::Asset().Warning("Asset '{}' not found", AssetGuid.ToString());
    }

    void DefaultStorage::RegisterHandler(
        size_t              Id,
        UPtr<IAssetHandler> Handler)
    {
        AME_LOG_ASSERT(Log::Asset(), Id, "Invalid handler id");
        if (!m_Handlers.emplace(Id, std::move(Handler)).second)
        {
            AME_LOG_ASSERT(Log::Asset(), false, "Handler already registered");
        }
    }

    void DefaultStorage::UnregisterHandler(
        size_t Id)
    {
        AME_LOG_ASSERT(Log::Asset(), Id > 0, "Invalid handler id");
        m_Handlers.erase(Id);
    }

    IAssetHandler* DefaultStorage::GetHandler(
        const Ptr<IAsset>& Asset,
        size_t*            Id)
    {
        for (auto& [HandlerId, Handler] : m_Handlers)
        {
            if (Handler->CanHandle(Asset))
            {
                if (HandlerId)
                {
                    *Id = HandlerId;
                }
                return Handler.get();
            }
        }

        return nullptr;
    }

    IAssetHandler* DefaultStorage::GetHandler(
        size_t Id)
    {
        AME_LOG_ASSERT(Log::Asset(), Id > 0, "Invalid handler id");

        auto Iter = m_Handlers.find(Id);
        return Iter != m_Handlers.end() ? Iter->second.get() : nullptr;
    }

    Manager& DefaultStorage::GetManager()
    {
        return m_Manager;
    }

    //

    IAssetPackage* DefaultStorage::Mount(
        UPtr<IAssetPackage> Package)
    {
        return m_Packages.emplace_back(std::move(Package)).get();
    }

    void DefaultStorage::Unmount(
        IAssetPackage* Package)
    {
        std::erase_if(
            m_Packages, [Package](const auto& CurPackage)
            { return CurPackage.get() == Package; });
    }

    //

    Co::generator<IAssetPackage*> DefaultStorage::GetPackages(
        const PackageFlags& Flags)
    {
        using namespace EnumBitOperators;

        auto Iter = m_Packages.begin();
        if ((Flags & PackageFlags::Memory) == PackageFlags::Memory)
        {
            co_yield Iter->get();
        }

        ++Iter;
        if ((Flags & PackageFlags::Disk) == PackageFlags::Disk) [[likely]]
        {
            for (; Iter != m_Packages.end(); ++Iter)
            {
                co_yield Iter->get();
            }
        }
    }

    auto DefaultStorage::GetAllAssets(
        const PackageFlags& Flags) -> Co::generator<PackageAndAsset>
    {
        for (auto& Packages : GetPackages(Flags))
        {
            for (auto& AssetGuid : Packages->GetAssets())
            {
                co_yield { Packages, AssetGuid };
            }
        }
    }
} // namespace Ame::Asset