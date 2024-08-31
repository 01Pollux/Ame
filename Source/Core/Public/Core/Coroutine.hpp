#pragma once

#include <Core/Ame.hpp>
#include <concurrencpp/concurrencpp.h>

#define AME_COROUTINE_INST(Type, Name)                                                                                 \
public:                                                                                                                \
    [[nodiscard]] const auto& Name() const                                                                             \
    {                                                                                                                  \
        return m_##Name;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    Ame::SharedPtr<Co::Type> m_##Name;

namespace Ame
{
    class Coroutine
    {
    public:
        static void Initialize()
        {
            s_Runtime = std::make_unique<Co::runtime>();
        }

        [[nodiscard]] static Co::runtime& Get() noexcept
        {
            return *s_Runtime;
        }

        static void Shutdown()
        {
            s_Runtime.reset();
        }

    private:
        static inline UniquePtr<Co::runtime> s_Runtime;
    };
} // namespace Ame

namespace concurrencpp
{
    /// <summary>
    /// Convert a generator to a container.
    /// </summary>
    template<template<typename... ArgsTy> typename ContainerTy, typename Ty>
    [[nodiscard]] auto generator_to_container(generator<Ty> gen)
    {
        ContainerTy<Ty> container{};
        for (auto v : gen)
        {
            container.emplace(container.end(), std::move(v));
        }
        return container;
    }

    /// <summary>
    /// Loop all tasks in the executor until it is empty.
    /// </summary>
    inline void manual_executor_loop_until_empty(const std::shared_ptr<manual_executor>& executor)
    {
        while (executor->size() > 0)
        {
            executor->loop(executor->size());
        }
    }

    /// <summary>
    /// Loop all tasks in the executors per chunk until all executors are empty or loopCount is zero.
    /// </summary>
    inline void manual_executor_loop_distribute(std::span<const std::shared_ptr<manual_executor>> executors,
                                                size_t maxLoopCount, size_t chunkCount = 1)
    {
        while (true)
        {
            bool dontBreak = false;
            for (auto& executor : executors)
            {
                if (maxLoopCount == 0)
                {
                    break;
                }

                size_t loopedCount = executor->loop(chunkCount);
                maxLoopCount -= loopedCount;
                dontBreak |= loopedCount != 0;
            }
            if (!dontBreak)
            {
                break;
            }
        }
    }

    /// <summary>
    /// Loop all tasks in the executors until all executors are empty.
    /// </summary>
    inline void manual_executor_loop_distribute_until_empty(std::span<const std::shared_ptr<manual_executor>> executors,
                                                            size_t chunkCount = 1)
    {
        while (true)
        {
            bool dontBreak = false;
            for (auto& executor : executors)
            {
                dontBreak |= executor->loop(chunkCount) != 0;
            }
            if (!dontBreak)
            {
                break;
            }
        }
    }
} // namespace concurrencpp
