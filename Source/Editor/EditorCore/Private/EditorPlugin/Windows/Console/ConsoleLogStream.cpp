#include <chrono>

#include <EditorPlugin/Windows/Console/ConsoleLogStream.hpp>
#include <Core/Enum.hpp>

#include <Log/Logger.hpp>

namespace Ame::Editor
{
    ConsoleLogStream::ConsoleLogStream(uint16_t capacity) : m_Logs(capacity)
    {
        using namespace EnumBitOperators;

        uint64_t mask = 0;
        if (Log::s_Logger)
        {
            auto level = Log::s_Logger->GetLevel();
            SetLevel(level);
            if (level == Log::LogLevel::Disabled)
            {
                return;
            }

            mask           = std::numeric_limits<uint64_t>::max();
            auto disableTo = std::to_underlying(level);

            // switch all levels from disableTo to lower bits in mask
            // mask = 1111 1111
            // disableTo = 2
            // mask = 1111 1100
            mask <<= disableTo;
        }
        SetLevelMask(mask);
    }

    void ConsoleLogStream::OnLog(const LogData& logData)
    {
        if (!IsLevelEnabled(logData.Level))
        {
            return;
        }

        auto message =
            std::format("[{0:%F} {0:%T}] [T: {1}] {2}", logData.Timepoint, logData.ThreadId, logData.Message);
        m_Logs.insert(m_Logs.end(), { std::move(message), logData.Level });
        m_ActiveLogsDirty = true;
    }

    //

    auto ConsoleLogStream::GetLevelMaskString() const noexcept -> const char*
    {
        return m_LevelMaskString.c_str();
    }

    auto ConsoleLogStream::GetLevelMask() const noexcept -> uint64_t
    {
        return m_LevelMask;
    }

    bool ConsoleLogStream::IsLevelEnabled(Log::LogLevel level) const noexcept
    {
        return (m_LevelMask & static_cast<uint64_t>(1ull << std::to_underlying(level))) != 0;
    }

    auto ConsoleLogStream::GetLevelColor(Log::LogLevel level) const noexcept -> Math::Color4
    {
        switch (level)
        {
        case Log::LogLevel::Trace:
            return Colors::c_GhostWhite;
        case Log::LogLevel::Debug:
            return Colors::c_Green;
        case Log::LogLevel::Info:
            return Colors::c_Cyan;
        case Log::LogLevel::Warning:
            return Colors::c_Yellow;
        case Log::LogLevel::Error:
            return Colors::c_Crimson;
        case Log::LogLevel::Fatal:
            return Colors::c_Red;
        default:
            std::unreachable();
        }
    }

    void ConsoleLogStream::SetLevelMask(uint64_t levelMask) noexcept
    {
        m_LevelMask = levelMask;
        UpdateLevelMaskString();
        m_ActiveLogsDirty = true;
    }

    void ConsoleLogStream::Clear()
    {
        m_Logs.clear();
    }

    //

    void ConsoleLogStream::UpdateActiveLogs()
    {
        if (!m_ActiveLogsDirty)
        {
            return;
        }

        m_ActiveLogsDirty = true;
        m_ActiveLogs.clear();
        for (uint16_t i = 0; i < m_Logs.size(); ++i)
        {
            if (IsLevelEnabled(m_Logs[i].Level))
            {
                m_ActiveLogs.push_back(i);
            }
        }
    }

    auto ConsoleLogStream::GetLogCount() const noexcept -> uint16_t
    {
        return static_cast<uint16_t>(m_ActiveLogs.size());
    }

    auto ConsoleLogStream::GetLog(uint16_t index) const noexcept -> const LogEntry&
    {
        index = m_ActiveLogs[index];
        return m_Logs[index];
    }

    //

    void ConsoleLogStream::UpdateLevelMaskString()
    {
        m_LevelMaskString.clear();
        for (auto level : Enum::enum_values<Log::LogLevel>())
        {
            if ((m_LevelMask & static_cast<uint64_t>(1ull << std::to_underlying(level))) == 0)
            {
                continue;
            }
            auto name = Enum::enum_name(level);
            if (!m_LevelMaskString.empty())
            {
                m_LevelMaskString += ", ";
            }
            m_LevelMaskString += name;
        }
        if (m_LevelMaskString.empty())
        {
            m_LevelMaskString = "None";
        }
    }
} // namespace Ame::Editor