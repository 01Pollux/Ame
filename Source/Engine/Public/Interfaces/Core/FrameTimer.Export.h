#pragma once

typedef struct Ame_FrameTimer_t Ame_FrameTimer_t;

// clang-format off
extern "C"
{
    // Must be released with Ame_IBaseObject_Release
    AME_ENGINE_C(Ame_FrameTimer_t*, Ame_FrameTimer_Create());

    AME_ENGINE_C(double, Ame_FrameTimer_GetGameTime(Ame_FrameTimer_t* frameTimerHandle));
    AME_ENGINE_C(double, Ame_FrameTimer_GetEngineTime(Ame_FrameTimer_t* frameTimerHandle));
    AME_ENGINE_C(double, Ame_FrameTimer_GetDeltaTime(Ame_FrameTimer_t* frameTimerHandle));
    AME_ENGINE_C(float, Ame_FrameTimer_GetTimeScale(Ame_FrameTimer_t* frameTimerHandle));
    AME_ENGINE_C(bool, Ame_FrameTimer_IsPaused(Ame_FrameTimer_t* frameTimerHandle));

    AME_ENGINE_C(void, Ame_FrameTimer_SetTimeScale(Ame_FrameTimer_t* frameTimerHandle, float timescale));
    AME_ENGINE_C(void, Ame_FrameTimer_Reset(Ame_FrameTimer_t* frameTimerHandle));
    AME_ENGINE_C(void, Ame_FrameTimer_Tick(Ame_FrameTimer_t* frameTimerHandle));
}
// clang-format on