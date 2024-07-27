#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    void Storage::RegisterHandler(
        const UId&         uid,
        Ptr<IAssetHandler> handler)
    {
        if (!m_Handlers.emplace(uid, std::move(handler)).second)
        {
            AME_LOG_ASSERT(Log::Asset(), false, "Handler already registered");
        }
    }

    void Storage::UnregisterHandler(
        const UId& uid)
    {
        m_Handlers.erase(uid);
    }

    std::pair<UId, IAssetHandler*> Storage::GetHandler(
        const Ptr<IAsset>& asset)
    {
        for (auto& [uid, handler] : m_Handlers)
        {
            if (handler->CanHandle(asset))
            {
                return { uid, handler };
            }
        }

        return {};
    }

    IAssetHandler* Storage::GetHandler(
        const UId& uid)
    {
        auto iter = m_Handlers.find(uid);
        return iter != m_Handlers.end() ? iter->second : nullptr;
    }
} // namespace Ame::Asset