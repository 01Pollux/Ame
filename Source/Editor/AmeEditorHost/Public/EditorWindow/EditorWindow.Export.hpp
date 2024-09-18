#pragma once

#include <Core/CApi.h>

typedef struct Ame_EditorWindow_t Ame_EditorWindow_t;
typedef void (*Ame_EditorWindow_OnDrawVisitble_t)();

// clang-format off
extern "C"
{
    // Must be released with Ame_IBaseObject_Release
    AME_EDITORHOST_C(Ame_EditorWindow_t*, Ame_EditorWindow_Create(Ame_StringView_t path));

    AME_EDITORHOST_C(void, Ame_EditorWindow_SetOnDrawVisible(Ame_StringView_t* window, void (AME_CDECL* callback)()));
    AME_EDITORHOST_C(void, Ame_EditorWindow_SetOnToolbarDraw(Ame_StringView_t* window, void (AME_CDECL* callback)()));
    AME_EDITORHOST_C(void, Ame_EditorWindow_SetOnShow(Ame_StringView_t* window, void (AME_CDECL* callback)()));
    AME_EDITORHOST_C(void, Ame_EditorWindow_SetOnHide(Ame_StringView_t* window, void (AME_CDECL* callback)()));

    AME_EDITORHOST_C(Ame_StringView_t, Ame_EditorWindow_GetFullPath(Ame_StringView_t* window));
}
// clang-format on
