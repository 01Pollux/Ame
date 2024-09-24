#include <Log/Stream.hpp>
#include <Log/Stream.Export.h>

#include <Log/Streams/CallbackStream.hpp>
#include <Log/Streams/ConsoleStream.hpp>
#include <Log/Streams/FileStream.hpp>
#include <Log/Streams/MsvcDebugStream.hpp>
#include <Log/Streams/NullStream.hpp>
#include <Log/Streams/RotatingFileStream.hpp>

//

void Ame_LoggerStream_Release(Ame_LoggerStream_t* streamHandle)
{
    delete std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
}

void Ame_LoggerStream_SetPattern(Ame_LoggerStream_t* streamHandle, Ame_StringView_t pattern)
{
    auto stream = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    stream->SetPattern({ pattern.Data, pattern.Size });
}

void Ame_LoggerStream_SetLevel(Ame_LoggerStream_t* streamHandle, char level)
{
    auto stream = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    stream->SetLevel(static_cast<Ame::Log::LogLevel>(level));
}

Ame_StringView_t Ame_LoggerStream_GetPattern(Ame_LoggerStream_t* streamHandle)
{
    auto  stream  = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    auto& pattern = stream->GetPattern();
    return { pattern.c_str(), pattern.size() };
}

char Ame_LoggerStream_GetLevel(Ame_LoggerStream_t* streamHandle)
{
    auto stream = std::bit_cast<Ame::Log::ILoggerStream*>(streamHandle);
    return static_cast<char>(stream->GetLevel());
}

//

class CallbackStreamWrapper : public Ame::Log::CallbackStream
{
public:
    void OnLog(const LogData& logData) override
    {
        Ame_Logger_LogData data{ .Message{ logData.Message.data(), logData.Message.size() },
                                 .ThreadId      = logData.ThreadId,
                                 .TimepointInMs = logData.Timepoint.time_since_epoch().count(),
                                 .Level         = static_cast<char>(logData.Level) };
        Callback(this, &data, UserData);
    }

public:
    Ame_LoggerStream_Callback_t Callback;
    void*                       UserData;
};

Ame_LoggerStream_t* Ame_LoggerStream_CreateCallback()
{
    auto stream = new CallbackStreamWrapper;
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}

void Ame_LoggerStream_UpdateCallback(Ame_LoggerStream_t* stream, Ame_LoggerStream_Callback_t callback, void* userData)
{
    auto streamWrapper      = static_cast<CallbackStreamWrapper*>(std::bit_cast<Ame::Log::ILoggerStream*>(stream));
    streamWrapper->Callback = callback;
    streamWrapper->UserData = userData;
}

Ame_LoggerStream_t* Ame_LoggerStream_CreateConsole()
{
    auto stream = new Ame::Log::ConsoleStream();
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}

Ame_LoggerStream_t* Ame_LoggerStream_CreateFile(Ame_StringView_t fileName, bool truncate)
{
    auto stream = new Ame::Log::FileStream({ fileName.Data, fileName.Size }, truncate);
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}

Ame_LoggerStream_t* Ame_LoggerStream_CreateMsvcDebug()
{
    auto stream = new Ame::Log::MsvcDebugStream();
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}

Ame_LoggerStream_t* Ame_LoggerStream_CreateNull()
{
    auto stream = new Ame::Log::NullStream();
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}

Ame_LoggerStream_t* Ame_LoggerStream_CreateRotatingFile(Ame_StringView_t baseFileName, size_t maxSize, size_t maxFiles,
                                                        bool rotateOnOpen)
{
    auto stream =
        new Ame::Log::RotatingFileStream({ baseFileName.Data, baseFileName.Size }, maxSize, maxFiles, rotateOnOpen);
    return std::bit_cast<Ame_LoggerStream_t*>(stream);
}