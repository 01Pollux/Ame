#pragma once

#include <Log/Stream.Export.h>

typedef struct Ame_Logger_t Ame_Logger_t;

// clang-format off
extern "C"
{
    // Must be released with Ame_IBaseObject_Release
    AME_ENGINE_C(Ame_Logger_t*, Ame_Logger_Create(Ame_StringView_t loggerName));

    AME_ENGINE_C(void, Ame_Logger_SetInstance(Ame_Logger_t* loggerHandle));
    AME_ENGINE_C(Ame_Logger_t*, Ame_Logger_GetInstance());

    AME_ENGINE_C(void, Ame_Logger_AddStream(Ame_Logger_t* loggerHandle, Ame_LoggerStream_t* streamHandle));
    AME_ENGINE_C(void, Ame_Logger_RemoveStream(Ame_Logger_t* loggerHandle, Ame_LoggerStream_t* streamHandle));

    AME_ENGINE_C(void, Ame_Logger_WriteMessage(Ame_Logger_t* loggerHandle, const Ame_LoggerInfo_t* logInfo));

    AME_ENGINE_C(char, Ame_Logger_GetLevel(Ame_Logger_t* loggerHandle));
    AME_ENGINE_C(void, Ame_Logger_SetLevel(Ame_Logger_t* loggerHandle, char level));
}
// clang-format on