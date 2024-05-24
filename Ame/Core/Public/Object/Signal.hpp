#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <set>

#include <Core/Ame.hpp>

namespace Ame
{
    static constexpr uint64_t c_InvalidSignalHandle = static_cast<uint64_t>(~0ull);

    template<typename... ArgsTy>
    class Signal
    {
    public:
        using DelegateType        = std::move_only_function<void(ArgsTy...)>;
        using CallbackDelegateMap = std::map<uint64_t, DelegateType>;

        struct Handle;

        using PendingAddList = CallbackDelegateMap;
        using PendingDropSet = std::set<uint64_t>;

    public:
        /// <summary>
        /// Add a listener to the delegate list
        /// </summary>
        template<std::invocable<ArgsTy...> FnTy>
        uint64_t Listen(
            FnTy&& listener)
        {
            std::scoped_lock addLock(m_AddOrDropMutex);
            uint64_t         id = m_NextHandle++;
            if (m_RunMutex.try_lock())
            {
                std::scoped_lock runLock(std::adopt_lock, m_RunMutex);
                m_Listeners.emplace(id, std::forward<FnTy>(listener));
                m_ListenerCount.fetch_add(1, std::memory_order_relaxed);
            }
            else
            {
                m_PendingAdd.emplace(id, std::forward<FnTy>(listener));
            }

            return id;
        }

        /// <summary>
        /// Remove a listener from the delegate list
        /// </summary>
        void Drop(
            uint64_t id)
        {
            std::scoped_lock dropLock(m_AddOrDropMutex);
            if (m_RunMutex.try_lock())
            {
                std::scoped_lock runLock(std::adopt_lock, m_RunMutex);
                m_Listeners.erase(id);
                m_ListenerCount.fetch_sub(1, std::memory_order_relaxed);
            }
            else
            {
                m_PendingAdd.erase(id);
                m_PendingDrop.insert(id);
            }
        }

        /// <summary>
        /// Invoke the listeners with the given arguments
        /// </summary>
        void Broadcast(
            ArgsTy&&... args)
        {
            // Skip broadcasting if there are no listeners
            if (m_ListenerCount.load(std::memory_order_relaxed) == 0)
            {
                return;
            }

            {
                std::scoped_lock runLock(m_RunMutex);
                for (auto& listener : m_Listeners)
                {
                    listener.second(std::forward<ArgsTy>(args)...);
                }
            }
            {
                std::scoped_lock addDropLock(m_AddOrDropMutex);

                m_Listeners.merge(m_PendingAdd);
                for (uint64_t id : m_PendingDrop)
                {
                    m_Listeners.erase(id);
                }
            }
        }

    private:
        std::atomic_uint64_t m_ListenerCount = 0;
        uint64_t             m_NextHandle    = 0;
        std::mutex           m_RunMutex, m_AddOrDropMutex;

        CallbackDelegateMap m_Listeners;
        PendingAddList      m_PendingAdd;
        PendingDropSet      m_PendingDrop;
    };

    //

    /// <summary>
    /// A signal that can be used as a static signal or an object signal
    /// </summary>
    template<typename... ArgsTy>
    class DoubleSignal
    {
    public:
        [[nodiscard]] auto& ObjectSignal() noexcept
        {
            return m_ObjectSignal;
        }

        [[nodiscard]] auto& StaticSignal() noexcept
        {
            return g_m_StaticSignal;
        }

        void Broadcast(
            ArgsTy&&... args)
        {
            m_ObjectSignal.Broadcast(std::forward<ArgsTy>(args)...);
            g_m_StaticSignal.Broadcast(std::forward<ArgsTy>(args)...);
        }

    private:
        Signal<ArgsTy...>               m_ObjectSignal;
        static inline Signal<ArgsTy...> g_m_StaticSignal;
    };

    //

    template<typename... ArgsTy>
    class SignalHandle
    {
    public:
        SignalHandle() = default;
        template<std::invocable<ArgsTy...> FnTy>
        SignalHandle(
            Signal<ArgsTy...>& sig,
            FnTy&&             delegate) :
            m_Signal(&sig),
            m_Id(m_Signal->Listen(std::forward<FnTy>(delegate)))
        {
        }

        SignalHandle(
            const SignalHandle& other) = delete;
        SignalHandle(
            SignalHandle&& other) noexcept :
            m_Signal(std::exchange(other.m_Signal, nullptr)),
            m_Id(std::exchange(other.m_Id, c_InvalidSignalHandle))
        {
        }

        SignalHandle& operator=(
            const SignalHandle& other) = delete;
        SignalHandle& operator=(
            SignalHandle&& other) noexcept
        {
            if (this != &other)
            {
                Drop();
                m_Signal = std::exchange(other.m_Signal, nullptr);
                m_Id     = std::exchange(other.m_Id, c_InvalidSignalHandle);
            }
            return *this;
        }

        ~SignalHandle()
        {
            Drop();
        }

        /// <summary>
        /// Attach the listener to the signal delegate list
        /// </summary>
        template<std::invocable<ArgsTy...> FnTy>
        void Attach(
            Signal<ArgsTy...>& sig,
            FnTy&&             delegate)
        {
            Drop();
            m_Signal = &sig;
            m_Id     = m_Signal->Listen(std::forward<FnTy>(delegate));
        }

        /// <summary>
        /// Release the ownership of the handle without dropping the callback
        /// </summary>
        void Release()
        {
            m_Signal = nullptr;
        }

        /// <summary>
        /// Drop the listener from the signal delegate list
        /// </summary>
        void Drop()
        {
            if (m_Signal)
            {
                m_Signal->Drop(m_Id);
                m_Id     = c_InvalidSignalHandle;
                m_Signal = nullptr;
            }
        }

    private:
        Signal<ArgsTy...>* m_Signal = nullptr;
        uint64_t           m_Id     = c_InvalidSignalHandle;
    };

    //

    template<typename... ArgsTy>
    struct Signal<ArgsTy...>::Handle
    {
    public:
        Handle() = default;

        template<std::invocable<ArgsTy...> FnTy>
        Handle(
            Signal<ArgsTy...>& sig,
            FnTy&&             delegate) :
            m_Handle(sig, std::forward<FnTy>(delegate))
        {
        }

        Handle(
            const Handle& other) = delete;
        Handle(
            Handle&& other) noexcept :
            m_Handle(std::move(other.m_Handle))
        {
        }

        Handle& operator=(
            const Handle& other) = delete;
        Handle& operator=(
            Handle&& other) noexcept
        {
            if (this != &other)
            {
                m_Handle = std::move(other.m_Handle);
            }
            return *this;
        }

        ~Handle() = default;

        /// <summary>
        /// Release the ownership of the handle without dropping the callback
        /// </summary>
        void Release()
        {
            m_Handle.Release();
        }

        /// <summary>
        /// Drop the listener from the signal delegate list
        /// </summary>
        void Drop()
        {
            m_Handle.Drop();
        }

    private:
        SignalHandle<ArgsTy...> m_Handle;
    };
} // namespace Ame

#define AME_SIGNAL_DECL(Name, ...)                        \
    namespace Ame::Signals                                \
    {                                                     \
        using Name      = Ame::Signal<__VA_ARGS__>;       \
        using Name##Dbl = Ame::DoubleSignal<__VA_ARGS__>; \
    }

#define AME_SIGNAL_INSTANCE_DECL(Class, Name, ...) AME_SIGNAL_DECL(Name, Class&, __VA_ARGS__)

#define AME_SIGNAL_INST(Name)                 \
public:                                       \
    [[nodiscard]] auto& Name() const noexcept \
    {                                         \
        return m_##Name;                      \
    }                                         \
                                              \
private:                                      \
    mutable Ame::Signals::Name m_##Name

#define AME_SIGNAL_STATIC(Name)                       \
public:                                               \
    [[nodiscard]] auto& Static##Name() const noexcept \
    {                                                 \
        return m_##Name;                              \
    }                                                 \
                                                      \
private:                                              \
    static inline Ame::Signals::Name m_##Name

#define AME_SIGNAL_DOUBLE(Name)               \
public:                                       \
    [[nodiscard]] auto& Name() const noexcept \
    {                                         \
        return m_##Name;                      \
    }                                         \
                                              \
private:                                      \
    mutable Ame::Signals::Name##Dbl m_##Name
