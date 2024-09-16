#include <Log/Logger.hpp>
#include <Log/Logger.Export.h>

Ame_Logger_t* Ame_Logger_Create(Ame_StringView_t loggerName)
{
    auto logger = Ame::Log::CreateLogger({ loggerName.Data, loggerName.Size }).Detach();
    return std::bit_cast<Ame_Logger_t*>(logger);
}

void Ame_Logger_SetInstance(Ame_Logger_t* loggerHandle)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    Ame::Log::ILogger::Initialize(Ame::Ptr{ logger });
}

Ame_Logger_t* Ame_Logger_GetInstance()
{
    auto logger = Ame::Log::ILogger::Get();
    return std::bit_cast<Ame_Logger_t*>(logger);
}

void Ame_Logger_AddStream(Ame_Logger_t* loggerHandle, const Ame_LoggerStream_t* streamHandle)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    auto stream = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    logger->AddStream(stream);
}

void Ame_Logger_RemoveStream(Ame_Logger_t* loggerHandle, const Ame_LoggerStream_t* streamHandle)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    auto stream = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    logger->RemoveStream(stream);
}

void Ame_Logger_WriteMessage(Ame_Logger_t* loggerHandle, const Ame_LoggerInfo_t* logInfo)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    logger->WriteMessage({ static_cast<Ame::Log::LogLevel>(logInfo->Level),
                           Ame::StringView{ logInfo->Message.Data, logInfo->Message.Size } });
}

char Ame_Logger_GetLevel(Ame_Logger_t* loggerHandle)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    return static_cast<char>(logger->GetLevel());
}

void Ame_Logger_SetLevel(Ame_Logger_t* loggerHandle, char level)
{
    auto logger = std::bit_cast<Ame::Log::ILogger*>(loggerHandle);
    logger->SetLevel(static_cast<Ame::Log::LogLevel>(level));
}