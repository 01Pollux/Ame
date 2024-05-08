#include <regex>
#include <ranges>
#include <Asset/Packs/Memory.hpp>

namespace Ame::Asset
{
    Co::generator<const Asset::Handle&> MemoryAssetPackage::GetAssets()
    {
        RLock Lock(m_CacheMutex);
        for (auto& Guid : m_Cache | std::views::keys)
        {
            co_yield Guid;
        }
    }

    bool MemoryAssetPackage::ContainsAsset(
        const Asset::Handle& AssetGuid) const
    {
        RLock Lock(m_CacheMutex);
        return m_Cache.contains(AssetGuid);
    }

    //

    Asset::Handle MemoryAssetPackage::FindAsset(
        const String& Path) const
    {
        RLock Lock(m_CacheMutex);
        for (auto& [Guid, Asset] : m_Cache)
        {
            if (Asset->GetPath() == Path)
            {
                return Guid;
            }
        }
        return Asset::Handle::Null;
    }

    Co::generator<const Asset::Handle&> MemoryAssetPackage::FindAssets(
        const std::regex& PathRegex) const
    {
        RLock Lock(m_CacheMutex);
        for (auto& [Guid, Asset] : m_Cache)
        {
            if (std::regex_match(Asset->GetPath(), PathRegex))
            {
                co_yield Asset::Handle{ Guid };
            }
        }
    }

    //

    Co::result<void> MemoryAssetPackage::Export()
    {
        co_return;
    }

    Co::result<void> MemoryAssetPackage::SaveAsset(
        Ptr<IAsset> Asset)
    {
        RWLock Lock(m_CacheMutex);
        m_Cache[Asset->GetGuid()] = std::move(Asset);
        co_return;
    }

    bool MemoryAssetPackage::RemoveAsset(
        const Asset::Handle& AssetGuid)
    {
        RWLock Lock(m_CacheMutex);
        return m_Cache.erase(AssetGuid) > 0;
    }

    Ptr<IAsset> MemoryAssetPackage::LoadAsset(
        const Asset::Handle& AssetGuid,
        bool)
    {
        RWLock Lock(m_CacheMutex);
        auto   Iter = m_Cache.find(AssetGuid);
        return Iter != m_Cache.end() ? Iter->second : nullptr;
    }

    bool MemoryAssetPackage::UnloadAsset(
        const Asset::Handle& AssetGuid,
        bool                 Force)
    {
        RWLock Lock(m_CacheMutex);

        auto Iter = m_Cache.find(AssetGuid);
        if (Iter == m_Cache.end())
        {
            return false;
        }

        if (!Force && Iter->second.use_count() == 1)
        {
            m_Cache.erase(Iter);
        }
        return true;
    }
} // namespace Ame::Asset
