#pragma once

#include <vector>
#include <boost/circular_buffer.hpp>

#include <Math/Colors.hpp>
#include <Log/Streams/CallbackStream.hpp>

namespace Ame::Editor
{
    class ConsoleLogStream : public Log::CallbackStream
    {
    public:
        struct LogEntry
        {
            String        Message;
            Log::LogLevel Level;
        };

        ConsoleLogStream(uint16_t capacity = 1024);

        void OnLog(const LogData& logData);

    public:
        [[nodiscard]] auto GetLevelMaskString() const noexcept -> const char*;
        [[nodiscard]] auto GetLevelMask() const noexcept -> uint64_t;
        [[nodiscard]] auto IsLevelEnabled(Log::LogLevel level) const noexcept -> bool;
        [[nodiscard]] auto GetLevelColor(Log::LogLevel level) const noexcept -> Math::Color4;

        void SetLevelMask(uint64_t levelMask) noexcept;
        void Clear();

    public:
        void               UpdateActiveLogs();
        [[nodiscard]] auto GetLogCount() const noexcept -> uint16_t;
        [[nodiscard]] auto GetLog(uint16_t index) const noexcept -> const LogEntry&;

    private:
        void UpdateLevelMaskString();

    private:
        uint64_t m_LevelMask = 0;
        String   m_LevelMaskString;

        boost::circular_buffer<LogEntry> m_Logs;
        std::vector<uint16_t>            m_ActiveLogs;
        bool                             m_ActiveLogsDirty = true;
    };
} // namespace Ame::Editor