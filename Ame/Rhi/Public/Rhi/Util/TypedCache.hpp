#pragma once

#include <unordered_map>
#include <Core/Ame.hpp>

namespace Ame::Rhi::Util
{
    // TODO: remove this or move it to /Private
    template<typename DescTy, typename DataTy>
    struct TypedCache

    {
    public:
        TypedCache() = default;

        TypedCache(const TypedCache&)            = delete;
        TypedCache& operator=(const TypedCache&) = delete;

        TypedCache(TypedCache&&)            = default;
        TypedCache& operator=(TypedCache&&) = default;

        ~TypedCache() = default;

    public:
        void Clear()
        {
            std::scoped_lock Guard(m_Mutex);
            m_Cache.clear();
        }

        template<typename FuncTy>
        [[nodiscard]] DataTy& Load(
            const DescTy& Desc,
            FuncTy        Func)
        {
            auto             Hash = std::hash<DescTy>{}(Desc);
            std::scoped_lock Guard(m_Mutex);

            auto& Data = m_Cache[Hash];
            if (!Data)
            {
                Data = Func(Hash, Desc);
            }
            return Data;
        }

    private:
        std::mutex                         m_Mutex;
        std::unordered_map<size_t, DataTy> m_Cache;
    };
} // namespace Ame::Rhi::Util