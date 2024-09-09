#pragma once

#include <Core/Ame.hpp>
#include <Core/Interface.hpp>
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
    // {6A81E4C8-A67E-44C0-9A06-03B3539B1F1D}
    inline constexpr UId IID_Coroutine{ 0x6a81e4c8, 0xa67e, 0x44c0, { 0x9a, 0x6, 0x3, 0xb3, 0x53, 0x9b, 0x1f, 0x1d } };

    class Coroutine : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Coroutine, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(Coroutine) : Base(counters)
        {
        }

    public:
        [[nodiscard]] static Co::runtime& Get() noexcept;

        static void Initialize();
        static void Shutdown();

    private:
        static inline Co::runtime m_Runtime;
    };

    inline Ptr<Coroutine> s_Coroutine;

    inline Co::runtime& Coroutine::Get() noexcept
    {
        return s_Coroutine->m_Runtime;
    }
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
