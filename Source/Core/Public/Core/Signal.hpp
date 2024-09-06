#pragma once

#include <boost/signals2.hpp>
#include <Core/Ame.hpp>
#include <Core/Export.hpp>

namespace Ame::Signals
{
    using Connection       = boost::signals2::connection;
    using ScopedConnection = boost::signals2::scoped_connection;
} // namespace Ame::Signals

#define AME_SIGNAL_DECL(Name, ...)                                                                                     \
    using Name##_Signal = boost::signals2::signal<__VA_ARGS__>;                                                        \
    using Name##_Slot   = Name##_Signal::slot_type;                                                                    \
    using Name##_SlotEx = Name##_Signal::extended_slot_type;

#define AME_SIGNAL_INST_N(Namespace, Name)                                                                             \
public:                                                                                                                \
    boost::signals2::connection Name(const Namespace::Name##_Signal::slot_type& slot)                                  \
    {                                                                                                                  \
        return m_##Name.connect(slot);                                                                                 \
    }                                                                                                                  \
    boost::signals2::connection Name##Ex(const Namespace::Name##_Signal::extended_slot_type& slot)                     \
    {                                                                                                                  \
        return m_##Name.connect_extended(slot);                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    template<typename... Args> auto Invoke_##Name(Args&&... args)                                                      \
    {                                                                                                                  \
        return m_##Name(std::forward<Args>(args)...);                                                                  \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    Namespace::Name##_Signal m_##Name

#define AME_SIGNAL_STATIC_N(Namespace, Name)                                                                           \
public:                                                                                                                \
    static boost::signals2::connection Name(const Namespace::Name##_Signal::slot_type& slot)                           \
    {                                                                                                                  \
        return s_##Name.connect(slot);                                                                                 \
    }                                                                                                                  \
    static boost::signals2::connection Name##Ex(const Namespace::Name##_Signal::extended_slot_type& slot)              \
    {                                                                                                                  \
        return s_##Name.connect_extended(slot);                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    template<typename... Args> auto Invoke_##Name(Args&&... args)                                                      \
    {                                                                                                                  \
        return s_##Name(std::forward<Args>(args)...);                                                                  \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    static inline Namespace##Name##_Signal s_##Name

#define AME_SIGNAL_INST(Name)   AME_SIGNAL_INST_N(Ame::Signals, Name)
#define AME_SIGNAL_STATIC(Name) AME_SIGNAL_STATIC_N(Ame::Signals, Name)
