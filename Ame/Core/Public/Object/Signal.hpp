#pragma once

#include <cstdint>
#include <utility>

#include <EASTL/list.h>
#include <EASTL/set.h>
#include <functional>
#include <mutex>

namespace Ame
{
    static constexpr uint64_t InvalidSignalHandle = static_cast<uint64_t>(~0ull);

    template<typename... ArgsTy>
    class Signal
    {
    public:
        using DelegateType = std::move_only_function<void(ArgsTy...)>;

        using CallbackDelegateData = std::pair<DelegateType, uint64_t>;
        using CallbackDelegateList = eastl::list<CallbackDelegateData>;

        using PendingAddList = CallbackDelegateList;
        using PendingDropSet = eastl::set<uint64_t>;

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
                m_Listeners.insert(m_Listeners.end(), { std::forward<FnTy>(Listener), Id });
            }
            else
            {
                m_PendingAdd.insert(m_PendingAdd.end(), { std::forward<FnTy>(Listener), Id });
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
                m_Listeners.remove_if([Id](const auto& Listener)
                                      { return Listener.second == Id; });
            }
            else
            {
                m_PendingDrop.insert(Id);
            }
        }

        /// <summary>
        /// Invoke the listeners with the given arguments
        /// </summary>
        void Broadcast(
            ArgsTy&&... Args)
        {
            {
                std::scoped_lock RunLock(m_RunMutex);
                for (auto& Listener : m_Listeners)
                {
                    Listener.first(std::forward<ArgsTy>(Args)...);
                }
            }
            {
                std::scoped_lock AddDropLock(m_AddOrDropMutex);

                m_Listeners.insert(m_Listeners.end(), std::make_move_iterator(m_PendingAdd.begin()), std::make_move_iterator(m_PendingAdd.end()));
                m_PendingAdd.clear();

                m_Listeners.remove_if([this](const auto& Listener)
                                      { return m_PendingDrop.find(Listener.second) != m_PendingDrop.end(); });
                m_PendingDrop.clear();
            }
        }

    private:
        uint64_t   m_NextHandle{ 0 };
        std::mutex m_RunMutex, m_AddOrDropMutex;

        CallbackDelegateList m_Listeners;
        PendingAddList       m_PendingAdd;
        PendingDropSet       m_PendingDrop;
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

        [[nodiscard]] static auto& GetStaticSignal() noexcept
        {
            return m_StaticSignal;
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
} // namespace Ame

#define AME_SIGNAL_DECL(Class, Name, ...)                         \
    namespace Ame::Signals                                        \
    {                                                             \
        using Name      = Ame::Signal<Class&, __VA_ARGS__>;       \
        using Name##Dbl = Ame::DoubleSignal<Class&, __VA_ARGS__>; \
        using Name##H   = Ame::SignalHandle<Class&, __VA_ARGS__>; \
        using Name##SH  = Ame::SignalHandle<Class&, __VA_ARGS__>; \
    }

#define AME_SIGNAL_INST(Name)           \
public:                                 \
    [[nodiscard]] auto& Name() noexcept \
    {                                   \
        return m_##Name;                \
    }                                   \
                                        \
private:                                \
    Ame::Signals::Name m_##Name

#define AME_SIGNAL_STATIC(Name)                 \
public:                                         \
    [[nodiscard]] auto& Static##Name() noexcept \
    {                                           \
        return m_##Name;                        \
    }                                           \
                                                \
private:                                        \
    static inline Ame::Signals::Name m_##Name

#define AME_SIGNAL_DOUBLE(Name)                 \
public:                                         \
    [[nodiscard]] auto& Static##Name() noexcept \
    {                                           \
        return m_##Name;                        \
    }                                           \
                                                \
private:                                        \
    Ame::Signals::Name##Dbl m_##Name
