#pragma once

#include <Log/Logger.hpp>
#include <spdlog/spdlog.h>

namespace Ame::Log
{
    class LoggerImpl : public BaseObject<ILogger>
    {
    public:
        using Base = BaseObject<ILogger>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Logger, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(LoggerImpl, String loggerName);

    public:
        ~LoggerImpl() override;

    public:
        void WriteMessage(const LoggerInfo& info) override;

        void AddStream(ILoggerStream* stream) override;
        void RemoveStream(ILoggerStream* stream) override;

        LogLevel GetLevel() const noexcept override;
        void     SetLevel(LogLevel level) noexcept override;

    private:
        spdlog::logger m_Logger;
    };
} // namespace Ame::Log
