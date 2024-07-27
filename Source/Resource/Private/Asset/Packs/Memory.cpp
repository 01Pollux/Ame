#include <regex>
#include <ranges>
#include <Asset/Packs/Memory.hpp>

namespace Ame::Asset
{
    Co::generator<UId> MemoryAssetPackage::GetAssets()
    {
        RLock readLock(m_CacheMutex);
        for (auto& uid : m_Cache | std::views::keys)
        {
            co_yield UId{ uid };
        }
    }

    bool MemoryAssetPackage::ContainsAsset(
        const UId& uid) const
    {
        RLock readLock(m_CacheMutex);
        return m_Cache.contains(uid);
    }

    //

    UId MemoryAssetPackage::FindAsset(
        const String& path) const
    {
        RLock readLock(m_CacheMutex);
        for (auto& [uid, asset] : m_Cache)
        {
            if (asset->GetPath() == path)
            {
                return uid;
            }
        }
        return UIdUtils::Null();
    }

    Co::generator<UId> MemoryAssetPackage::FindAssets(
        const std::regex& pathRegex) const
    {
        RLock readLock(m_CacheMutex);
        for (auto& [uid, asset] : m_Cache)
        {
            if (std::regex_match(asset->GetPath(), pathRegex))
            {
                co_yield UId{ uid };
            }
        }
    }

    //

    Co::result<void> MemoryAssetPackage::Export()
    {
        co_return;
    }

    Co::result<void> MemoryAssetPackage::SaveAsset(
        Ptr<IAsset> asset)
    {
        RWLock readWriteLock(m_CacheMutex);
        m_Cache[asset->GetUId()] = std::move(asset);
        co_return;
    }

    bool MemoryAssetPackage::RemoveAsset(
        const UId& uid)
    {
        RWLock readWriteLock(m_CacheMutex);
        return m_Cache.erase(uid) > 0;
    }

    Ptr<IAsset> MemoryAssetPackage::LoadAsset(
        const UId& uid,
        bool)
    {
        RWLock readWriteLock(m_CacheMutex);
        auto   iter = m_Cache.find(uid);
        return iter != m_Cache.end() ? iter->second : Ptr<IAsset>{};
    }

    bool MemoryAssetPackage::UnloadAsset(
        const UId& uid,
        bool       force)
    {
        RWLock readWriteLock(m_CacheMutex);

        auto iter = m_Cache.find(uid);
        if (iter == m_Cache.end())
        {
            return false;
        }

        if (!force && iter->second->GetReferenceCounters()->GetNumStrongRefs() == 1)
        {
            m_Cache.erase(iter);
        }
        return true;
    }
} // namespace Ame::Asset
