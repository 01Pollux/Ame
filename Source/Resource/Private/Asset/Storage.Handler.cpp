#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    void Storage::RegisterHandler(
        size_t             id,
        Ptr<IAssetHandler> handler)
    {
        AME_LOG_ASSERT(Log::Asset(), id, "Invalid handler id");
        if (!m_Handlers.emplace(id, std::move(handler)).second)
        {
            AME_LOG_ASSERT(Log::Asset(), false, "Handler already registered");
        }
    }

    void Storage::UnregisterHandler(
        size_t id)
    {
        m_Handlers.erase(id);
    }

    std::pair<IAssetHandler*, size_t> Storage::GetHandler(
        const Ptr<IAsset>& asset)
    {
        for (auto& [id, handler] : m_Handlers)
        {
            if (handler->CanHandle(asset))
            {
                return { handler, id };
            }
        }

        return {};
    }

    IAssetHandler* Storage::GetHandler(
        size_t id)
    {
        AME_LOG_ASSERT(Log::Asset(), id > 0, "Invalid handler id");

        auto iter = m_Handlers.find(id);
        return iter != m_Handlers.end() ? iter->second : nullptr;
    }
} // namespace Ame::Asset