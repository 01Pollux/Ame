#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Logger.hpp>

namespace Ame::Asset
{
    void Storage::RegisterHandler(const UId& uid, Ptr<IAssetHandler> handler)
    {
        AME_LOG_ASSERT(m_Handlers.emplace(uid, std::move(handler)).second, "Handler already registered");
    }

    void Storage::UnregisterHandler(const UId& uid)
    {
        m_Handlers.erase(uid);
    }

    std::pair<UId, IAssetHandler*> Storage::GetHandler(IAsset* asset)
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

    IAssetHandler* Storage::GetHandler(const UId& uid)
    {
        auto iter = m_Handlers.find(uid);
        return iter != m_Handlers.end() ? iter->second : nullptr;
    }
} // namespace Ame::Asset