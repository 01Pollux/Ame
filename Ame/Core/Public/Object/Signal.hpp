#pragma once

#include <cstdint>
#include <utility>

#include <EASTL/list.h>
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

        using PendingAddList  = CallbackDelegateList;
        using PendingDropList = eastl::vector<uint64_t>;

    public:
        /// <summary>
        /// Add a listener to the delegate list
        /// </summary>
        template<std::invocable<ArgsTy...> FnTy>
        uint64_t Listen(
            FnTy&& Listener)
        {
            std::scoped_lock AddLock(m_AddOrDropMutex);
            if (m_RunMutex.try_lock())
            {
                std::scoped_lock RunLock(std::adopt_lock, m_RunMutex);
                m_Listeners.insert(m_Listeners.end(), { std::forward<FnTy>(Listener), m_NextHandle++ });
            }
            else
            {
                m_PendingAdd.insert(m_PendingAdd.end(), { std::forward<FnTy>(Listener), m_NextHandle++ });
            }
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
                std::erase_if(
                    m_Listeners,
                    [Id](const auto& Listener)
                    { return Listener.second == Id; });
            }
            else
            {
                m_PendingDrop.insert(m_PendingDrop.end(), Id);
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
                    Listener.first(std::move(Args)...);
                }
            }
            {
                std::scoped_lock AddDropLock(m_AddOrDropMutex);

                m_Listeners.insert(m_Listeners.end(), std::make_move_iterator(m_PendingAdd.begin()), std::make_move_iterator(m_PendingAdd.end()));
                m_PendingAdd.clear();

                for (uint64_t Id : m_PendingDrop)
                {
                    std::erase_if(
                        m_Listeners,
                        [Id](const auto& Listener)
                        { return Listener.second == Id; });
                }
                m_PendingDrop.clear();
            }
        }

    private:
        uint64_t   m_NextHandle{ 0 };
        std::mutex m_RunMutex, m_AddOrDropMutex;

        CallbackDelegateList m_Listeners;
        PendingAddList       m_PendingAdd;
        PendingDropList      m_PendingDrop;
    };

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
        Util::Signal<ArgsTy...>* m_Signal = nullptr;
        uint64_t                 m_Id     = InvalidSignalHandle;
    };
} // namespace Ame

#define AME_SIGNAL_DECL(Name, ...)                       \
    namespace Ame::Signals                               \
    {                                                    \
        using S##Name  = Ame::Signal<__VA_ARGS__>;       \
        using SH##Name = Ame::SignalHandle<__VA_ARGS__>; \
    }

#define AME_SIGNAL_INST(Name)           \
public:                                 \
    [[nodiscard]] auto& Name() noexcept \
    {                                   \
        return m_##Name;                \
    }                                   \
                                        \
private:                                \
    Ame::Signals::S##Name m_##Name
