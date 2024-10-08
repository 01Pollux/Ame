#pragma once

typedef struct Ame_LoggerStream_t Ame_LoggerStream_t;

// Must match the LogLevel enum in Core.hpp
#define AME_LOGLEVEL_TRACE 0
#define AME_LOGLEVEL_DEBUG 1
#define AME_LOGLEVEL_INFO  2
#define AME_LOGLEVEL_WARN  3
#define AME_LOGLEVEL_ERROR 4
#define AME_LOGLEVEL_FATAL 5
#define AME_LOGLEVEL_OFF   6

typedef struct Ame_LoggerInfo_t
{
    Ame_StringView_t Message;
    char             Level;
} Ame_LoggerInfo_t;

typedef struct Ame_Logger_LogData
{
    Ame_StringView_t Message;
    size_t           ThreadId;
    int64_t          TimepointInMs;
    char             Level;
} Ame_Logger_LogData;

typedef void(AME_CDECL* Ame_LoggerStream_Callback_t)(void* /*stream*/, const Ame_Logger_LogData* /*data*/,
                                                     void* /*userData*/);

// clang-format off
extern "C"
{
    AME_ENGINE_C(void, Ame_LoggerStream_Release(Ame_LoggerStream_t* stream));

    AME_ENGINE_C(void, Ame_LoggerStream_SetPattern(Ame_LoggerStream_t* stream, Ame_StringView_t pattern));
    AME_ENGINE_C(void, Ame_LoggerStream_SetLevel(Ame_LoggerStream_t* stream, char level));

    AME_ENGINE_C(Ame_StringView_t, Ame_LoggerStream_GetPattern(Ame_LoggerStream_t* stream, Ame_StringView_t pattern));
    AME_ENGINE_C(char, Ame_LoggerStream_GetLevel(Ame_LoggerStream_t* stream, char level));

    //

    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateCallback());
    AME_ENGINE_C(void, Ame_LoggerStream_UpdateCallback(Ame_LoggerStream_t* stream, Ame_LoggerStream_Callback_t callback, void* userData));
    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateConsole());
    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateFile(Ame_StringView_t fileName, bool truncate));
    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateMsvcDebug());
    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateNull());
    AME_ENGINE_C(Ame_LoggerStream_t*, Ame_LoggerStream_CreateRotatingFile(Ame_StringView_t baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen));
}
// clang-format on