#include <Interfaces/Core/FrameTimer.hpp>
#include <Interfaces/Core/FrameTimer.Export.h>

Ame_FrameTimer_t* Ame_FrameTimer_Create()
{
    return std::bit_cast<Ame_FrameTimer_t*>(AmeCreateRaw(Ame::Interfaces::FrameTimer));
}

double Ame_FrameTimer_GetGameTime(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    return frameTimer->GetGameTime();
}

double Ame_FrameTimer_GetEngineTime(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    return frameTimer->GetEngineTime();
}

double Ame_FrameTimer_GetDeltaTime(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    return frameTimer->GetDeltaTime();
}

float Ame_FrameTimer_GetTimeScale(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    return frameTimer->GetTimeScale();
}

bool Ame_FrameTimer_IsPaused(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    return frameTimer->IsPaused();
}

void Ame_FrameTimer_SetTimeScale(Ame_FrameTimer_t* frameTimerHandle, float timescale)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    frameTimer->SetTimeScale(timescale);
}

void Ame_FrameTimer_Reset(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    frameTimer->Reset();
}

void Ame_FrameTimer_Tick(Ame_FrameTimer_t* frameTimerHandle)
{
    auto frameTimer = std::bit_cast<Ame::Interfaces::FrameTimer*>(frameTimerHandle);
    frameTimer->Tick();
}