#pragma once

#include <boost/signals2.hpp>
#include <Core/Ame.hpp>

namespace Ame::Signals
{
    using Connection       = boost::signals2::connection;
    using ScopedConnection = boost::signals2::scoped_connection;
} // namespace Ame::Signals

#define AME_SIGNAL_DECL(Name, ...)                              \
    using Name##_Signal = boost::signals2::signal<__VA_ARGS__>; \
    using Name##_Slot   = Name##_Signal::slot_type;             \
    using Name##_SlotEx = Name##_Signal::extended_slot_type;

#define AME_SIGNAL_INST(Name)                              \
public:                                                    \
    boost::signals2::connection Name(                      \
        Name##_Slot slot)                                  \
    {                                                      \
        return m_##Name.connect(std::move(slot));          \
    }                                                      \
    boost::signals2::connection Name##Ex(                  \
        Name##_SlotEx slot)                                \
    {                                                      \
        return m_##Name.connect_extended(std::move(slot)); \
    }                                                      \
                                                           \
private:                                                   \
    Name##_Signal m_##Name

#define AME_SIGNAL_STATIC(Name)                            \
public:                                                    \
    static boost::signals2::connection Name(               \
        Name##_Slot slot)                                  \
    {                                                      \
        return m_##Name.connect(std::move(slot));          \
    }                                                      \
    static boost::signals2::connection Name##Ex(           \
        Name##_SlotEx slot)                                \
    {                                                      \
        return m_##Name.connect_extended(std::move(slot)); \
    }                                                      \
                                                           \
private:                                                   \
    static inline Name##_Signal m_##Name
