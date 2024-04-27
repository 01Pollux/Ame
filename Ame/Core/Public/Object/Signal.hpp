#pragma once

#include <map>
#include <set>
#include <functional>
#include <mutex>
#include <atomic>

#include <Core/Ame.hpp>

namespace Ame
{
    static constexpr uint64_t InvalidSignalHandle = static_cast<uint64_t>(~0ull);

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
            FnTy&& Listener)
        {
            std::scoped_lock AddLock(m_AddOrDropMutex);
            uint64_t         Id = m_NextHandle++;
            if (m_RunMutex.try_lock())
            {
                std::scoped_lock RunLock(std::adopt_lock, m_RunMutex);
                m_Listeners.emplace(Id, std::forward<FnTy>(Listener));
                m_ListenerCount.fetch_add(1, std::memory_order_relaxed);
            }
            else
            {
                m_PendingAdd.emplace(Id, std::forward<FnTy>(Listener));
            }

            return Id;
        }

        /// <summary>
        /// Remove a listener from the delegate list
        /// </summary>
        void Drop(
            uint64_t Id)
        {
            std::scoped_lock DropLock(m_AddOrDropMutex);
            if (m_RunMutex.try_lock())
            {
                std::scoped_lock RunLock(std::adopt_lock, m_RunMutex);
                m_Listeners.erase(Id);
                m_ListenerCount.fetch_sub(1, std::memory_order_relaxed);
            }
            else
            {
                m_PendingAdd.erase(Id);
                m_PendingDrop.insert(Id);
            }
        }

        /// <summary>
        /// Invoke the listeners with the given arguments
        /// </summary>
        void Broadcast(
            ArgsTy&&... Args)
        {
            // Skip broadcasting if there are no listeners
            if (m_ListenerCount.load(std::memory_order_relaxed) == 0)
            {
                return;
            }

            {
                std::scoped_lock RunLock(m_RunMutex);
                for (auto& Listener : m_Listeners)
                {
                    Listener.second(std::forward<ArgsTy>(Args)...);
                }
            }
            {
                std::scoped_lock AddDropLock(m_AddOrDropMutex);

                m_Listeners.merge(m_PendingAdd);
                for (uint64_t Id : m_PendingDrop)
                {
                    m_Listeners.erase(Id);
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
            return m_StaticSignal;
        }

        void Broadcast(
            ArgsTy&&... Args)
        {
            m_ObjectSignal.Broadcast(std::forward<ArgsTy>(Args)...);
            m_StaticSignal.Broadcast(std::forward<ArgsTy>(Args)...);
        }

    private:
        Signal<ArgsTy...>               m_ObjectSignal;
        static inline Signal<ArgsTy...> m_StaticSignal;
    };

    //

    template<typename... ArgsTy>
    class SignalHandle
    {
    public:
        SignalHandle() = default;
        template<std::invocable<ArgsTy...> FnTy>
        SignalHandle(
            Signal<ArgsTy...>& Sig,
            FnTy&&             Delegate) :
            m_Signal(&Sig),
            m_Id(m_Signal->Listen(std::forward<FnTy>(Delegate)))
        {
        }

        SignalHandle(
            const SignalHandle& Other) = delete;
        SignalHandle(
            SignalHandle&& Other) noexcept :
            m_Signal(std::exchange(Other.m_Signal, nullptr)),
            m_Id(std::exchange(Other.m_Id, InvalidSignalHandle))
        {
        }

        SignalHandle& operator=(
            const SignalHandle& Other) = delete;
        SignalHandle& operator=(
            SignalHandle&& Other) noexcept
        {
            if (this != &Other)
            {
                Drop();
                m_Signal = std::exchange(Other.m_Signal, nullptr);
                m_Id     = std::exchange(Other.m_Id, InvalidSignalHandle);
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
            Signal<ArgsTy...>& Sig,
            FnTy&&             Delegate)
        {
            Drop();
            m_Signal = &Sig;
            m_Id     = m_Signal->Listen(std::forward<FnTy>(Delegate));
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
                m_Id     = InvalidSignalHandle;
                m_Signal = nullptr;
            }
        }

    private:
        Signal<ArgsTy...>* m_Signal = nullptr;
        uint64_t           m_Id     = InvalidSignalHandle;
    };

    //

    template<typename... ArgsTy>
    struct Signal<ArgsTy...>::Handle
    {
    public:
        Handle() = default;

        template<std::invocable<ArgsTy...> FnTy>
        Handle(
            Signal<ArgsTy...>& Sig,
            FnTy&&             Delegate) :
            m_Handle(Sig, std::forward<FnTy>(Delegate))
        {
        }

        Handle(
            const Handle& Other) = delete;
        Handle(
            Handle&& Other) noexcept :
            m_Handle(std::move(Other.m_Handle))
        {
        }

        Handle& operator=(
            const Handle& Other) = delete;
        Handle& operator=(
            Handle&& Other) noexcept
        {
            if (this != &Other)
            {
                m_Handle = std::move(Other.m_Handle);
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
