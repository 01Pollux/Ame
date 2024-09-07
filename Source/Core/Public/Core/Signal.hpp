#pragma once

#include <boost/signals2.hpp>
#include <Core/Ame.hpp>
#include <Core/Export.hpp>

namespace Ame::Signals
{
    using Connection       = boost::signals2::connection;
    using ScopedConnection = boost::signals2::scoped_connection;

    template<typename... SigTy> class Signal
    {
    public:
        using base_type = boost::signals2::signal<SigTy...>;
        using slot_type = typename base_type::slot_type;
        using slot_ex   = typename base_type::extended_slot_type;

    private:
        struct ISignal
        {
            virtual ~ISignal()                                = default;
            virtual Connection Connect(const slot_type& slot) = 0;
            virtual Connection ConnectEx(const slot_ex& slot) = 0;
        };

        struct SignalImpl : ISignal
        {
            [[nodiscard]] Connection Connect(const slot_type& slot) override
            {
                return m_Signal.connect(slot);
            }
            [[nodiscard]] Connection ConnectEx(const slot_ex& slot) override
            {
                return m_Signal.connect_extended(slot);
            }
            base_type m_Signal;
        };

    public:
        Connection Connect(const slot_type& slot)
        {
            return m_Signal->Connect(slot);
        }

        Connection ConnectEx(const slot_ex& slot)
        {
            return m_Signal->ConnectEx(slot);
        }

        template<typename... Args> auto Invoke(Args&&... args)
        {
            auto impl = static_cast<SignalImpl*>(m_Signal.get());
            return impl->m_Signal(std::forward<Args>(args)...);
        }

    private:
        UniquePtr<ISignal> m_Signal = std::make_unique<SignalImpl>();
    };
} // namespace Ame::Signals

#define AME_SIGNAL_DECL(Name, ...) using Name##_Signal = Ame::Signals::Signal<__VA_ARGS__>;
