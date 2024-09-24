#include <EditorWindow/EditorWindow.hpp>
#include <EditorWindow/EditorWindow.Export.h>

using namespace Ame;

class EditorWindowImpl : public Editor::IEditorWindow
{
public:
    // {DBF9A2D2-0D13-4BEC-A1ED-79008DDCAB02}
    static inline constexpr UId IID_EditorWindowImpl{
        0xdbf9a2d2, 0xd13, 0x4bec, { 0xa1, 0xed, 0x79, 0x0, 0x8d, 0xdc, 0xab, 0x2 }
    };

    IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_EditorWindowImpl, IEditorWindow);

private:
    IMPLEMENT_INTERFACE_CTOR(EditorWindowImpl, Ame_StringView_t path) :
        IEditorWindow(counters, String{ path.Data, path.Size })
    {
    }

public:
    void OnDrawVisible() override
    {
        if (OnDrawVisibleFn)
        {
            OnDrawVisibleFn(this);
        }
    }

    void OnToolbarDraw() override
    {
        if (OnToolbarDrawFn)
        {
            OnToolbarDrawFn(this);
        }
    }

    void OnShow() override
    {
        if (OnShowFn)
        {
            OnShowFn(this);
        }
    }

    void OnHide() override
    {
        if (OnHideFn)
        {
            OnHideFn(this);
        }
    }

public:
    Ame_EditorWindow_Callback_t OnDrawVisibleFn = nullptr;
    Ame_EditorWindow_Callback_t OnToolbarDrawFn = nullptr;
    Ame_EditorWindow_Callback_t OnShowFn        = nullptr;
    Ame_EditorWindow_Callback_t OnHideFn        = nullptr;
};

//

Ame_EditorWindow_t* Ame_EditorWindow_Create(Ame_StringView_t path)
{
    return std::bit_cast<Ame_EditorWindow_t*>(AmeCreateRaw(EditorWindowImpl, path));
}

void Ame_EditorWindow_SetOnDrawVisible(Ame_EditorWindow_t* window, Ame_EditorWindow_Callback_t callback)
{
    auto impl             = std::bit_cast<EditorWindowImpl*>(window);
    impl->OnDrawVisibleFn = callback;
}

void Ame_EditorWindow_SetOnToolbarDraw(Ame_EditorWindow_t* window, Ame_EditorWindow_Callback_t callback)
{
    auto impl             = std::bit_cast<EditorWindowImpl*>(window);
    impl->OnToolbarDrawFn = callback;
}

void Ame_EditorWindow_SetOnShow(Ame_EditorWindow_t* window, Ame_EditorWindow_Callback_t callback)
{
    auto impl      = std::bit_cast<EditorWindowImpl*>(window);
    impl->OnShowFn = callback;
}

void Ame_EditorWindow_SetOnHide(Ame_EditorWindow_t* window, Ame_EditorWindow_Callback_t callback)
{
    auto impl      = std::bit_cast<EditorWindowImpl*>(window);
    impl->OnHideFn = callback;
}

Ame_StringView_t Ame_EditorWindow_GetFullPath(Ame_EditorWindow_t* window)
{
    auto impl = std::bit_cast<EditorWindowImpl*>(window);
    return { impl->GetFullPath().c_str(), impl->GetFullPath().size() };
}
