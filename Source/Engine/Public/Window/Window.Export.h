#pragma once

typedef struct Ame_IWindow_t Ame_IWindow_t;

typedef struct Ame_WindowCreateDesc_t
{
    const char* Title;

    int Width;
    int Height;

    bool CustomTitleBar;
    bool StartInMiddle;
    bool FullScreen;
    bool Maximized;
    bool NoResize;
} Ame_WindowCreateDesc_t;

inline void Ame_WindowCreateDesc_Init(Ame_WindowCreateDesc_t* desc)
{
    desc->Title = nullptr;

    desc->Width  = 800;
    desc->Height = 600;

    desc->CustomTitleBar = false;
    desc->StartInMiddle  = true;
    desc->FullScreen     = false;
    desc->Maximized      = false;
    desc->NoResize       = false;
}

typedef enum Ame_WindowType_t
{
    AME_WINDOW_TYPE_DESKTOP,
} Ame_WindowType_t;

// clang-format off
extern "C"
{
    // Must be released with Ame_IBaseObject_Release
    AME_ENGINE_C(Ame_IWindow_t*, Ame_IWindow_Create(Ame_WindowType_t windowType, const Ame_WindowCreateDesc_t* createDesc));

    AME_ENGINE_C(bool, Ame_IWindow_IsRunning(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_Close(Ame_IWindow_t* window));

    AME_ENGINE_C(void, Ame_IWindow_SetTitle(Ame_IWindow_t* window, Ame_StringView_t title));
    AME_ENGINE_C(void, Ame_IWindow_SetSize(Ame_IWindow_t* window, int width, int height));
    AME_ENGINE_C(void, Ame_IWindow_SetPosition(Ame_IWindow_t* window, int x, int y));

    AME_ENGINE_C(Ame_StringView_t, Ame_IWindow_GetTitle(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_GetSize(Ame_IWindow_t* window, int* width, int* height));
    AME_ENGINE_C(void, Ame_IWindow_GetPosition(Ame_IWindow_t* window, int* x, int* y));

    AME_ENGINE_C(bool, Ame_IWindow_IsFullScreen(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_SetFullscreen(Ame_IWindow_t* window, bool state));

    AME_ENGINE_C(bool, Ame_IWindow_IsMinimized(Ame_IWindow_t* window));
    AME_ENGINE_C(bool, Ame_IWindow_IsMaximized(Ame_IWindow_t* window));

    AME_ENGINE_C(void, Ame_IWindow_Maximize(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_Minimize(Ame_IWindow_t* window));

    AME_ENGINE_C(bool, Ame_IWindow_IsVisible(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_SetVisible(Ame_IWindow_t* window, bool show));

    AME_ENGINE_C(bool, Ame_IWindow_HasFocus(Ame_IWindow_t* window));
    AME_ENGINE_C(void, Ame_IWindow_RequestFocus(Ame_IWindow_t* window));

    AME_ENGINE_C(void, Ame_IWindow_Restore(Ame_IWindow_t* window));
}
// clang-format on