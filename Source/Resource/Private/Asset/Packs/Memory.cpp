#include <regex>
#include <ranges>
#include <Asset/Packs/Memory.hpp>

namespace Ame::Asset
{
    Co::generator<Guid> MemoryAssetPackage::GetAssets()
    {
        RLock readLock(m_CacheMutex);
        for (auto& guid : m_Cache | std::views::keys)
        {
            co_yield Guid{ guid };
        }
    }

    bool MemoryAssetPackage::ContainsAsset(
        const Guid& guid) const
    {
        RLock readLock(m_CacheMutex);
        return m_Cache.contains(guid);
    }

    //

    Guid MemoryAssetPackage::FindAsset(
        const String& path) const
    {
        RLock readLock(m_CacheMutex);
        for (auto& [guid, asset] : m_Cache)
        {
            if (asset->GetPath() == path)
            {
                return guid;
            }
        }
        return Guid::c_Null;
    }

    Co::generator<Guid> MemoryAssetPackage::FindAssets(
        const std::regex& pathRegex) const
    {
        RLock readLock(m_CacheMutex);
        for (auto& [guid, asset] : m_Cache)
        {
            if (std::regex_match(asset->GetPath(), pathRegex))
            {
                co_yield Guid{ guid };
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
        m_Cache[asset->GetGuid()] = std::move(asset);
        co_return;
    }

    bool MemoryAssetPackage::RemoveAsset(
        const Guid& guid)
    {
        RWLock readWriteLock(m_CacheMutex);
        return m_Cache.erase(guid) > 0;
    }

    Ptr<IAsset> MemoryAssetPackage::LoadAsset(
        const Guid& guid,
        bool)
    {
        RWLock readWriteLock(m_CacheMutex);
        auto   iter = m_Cache.find(guid);
        return iter != m_Cache.end() ? iter->second : Ptr<IAsset>{};
    }

    bool MemoryAssetPackage::UnloadAsset(
        const Guid& guid,
        bool        force)
    {
        RWLock readWriteLock(m_CacheMutex);

        auto iter = m_Cache.find(guid);
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
