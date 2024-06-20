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
            std::scoped_lock cacheLock(m_Mutex);
            m_Cache.clear();
        }

        template<typename FuncTy>
        [[nodiscard]] DataTy& Load(
            const DescTy& desc,
            FuncTy        initFunc)
        {
            auto             hash = std::hash<DescTy>{}(desc);
            std::scoped_lock cacheLock(m_Mutex);

            auto& data = m_Cache[hash];
            if (!data)
            {
                data = initFunc(hash, desc);
            }
            return data;
        }

    private:
        std::mutex                         m_Mutex;
        std::unordered_map<size_t, DataTy> m_Cache;
    };
} // namespace Ame::Rhi::Util