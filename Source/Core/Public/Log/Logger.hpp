#pragma once

#include <Log/Core.hpp>
#include <Core/Interface.hpp>

namespace Ame::Log
{
    // {940017D2-269C-45B6-803B-F3C530151CCA}
    inline constexpr UId IID_Logger{ 0x940017d2, 0x269c, 0x45b6, { 0x80, 0x3b, 0xf3, 0xc5, 0x30, 0x15, 0x1c, 0xca } };

    class ILogger : public IObject
    {
    public:
        virtual void WriteMessage(const LoggerInfo& info) = 0;

        virtual void AddStream(ILoggerStream* stream)    = 0;
        virtual void RemoveStream(ILoggerStream* stream) = 0;

        [[nodiscard]] virtual LogLevel GetLevel() const noexcept         = 0;
        virtual void                   SetLevel(LogLevel level) noexcept = 0;

        [[nodiscard]] bool CanLog([[maybe_unused]] LogLevel level) const noexcept
        {
#ifdef AME_DISABLE_LOGGING
            return false;
#else
            LogLevel CurLevel = GetLevel();
#ifdef AME_DIST
            if (CurLevel < LogLevel::Warning)
            {
                return false;
            }
#endif
            return CurLevel >= level;
#endif
        }
    };

    //

    inline Ptr<ILogger> Logger;

    [[nodiscard]] Ptr<ILogger> CreateLogger(String name);
} // namespace Ame::Log
