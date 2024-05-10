#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    void Storage::RegisterHandler(
        size_t              Id,
        UPtr<IAssetHandler> Handler)
    {
        AME_LOG_ASSERT(Log::Asset(), Id, "Invalid handler id");
        if (!m_Handlers.emplace(Id, std::move(Handler)).second)
        {
            AME_LOG_ASSERT(Log::Asset(), false, "Handler already registered");
        }
    }

    void Storage::UnregisterHandler(
        size_t Id)
    {
        AME_LOG_ASSERT(Log::Asset(), Id > 0, "Invalid handler id");
        m_Handlers.erase(Id);
    }

    IAssetHandler* Storage::GetHandler(
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

    IAssetHandler* Storage::GetHandler(
        size_t Id)
    {
        AME_LOG_ASSERT(Log::Asset(), Id > 0, "Invalid handler id");

        auto Iter = m_Handlers.find(Id);
        return Iter != m_Handlers.end() ? Iter->second.get() : nullptr;
    }
} // namespace Ame::Asset