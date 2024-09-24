#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class GameViewEditorWindow : public IEditorWindow
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_GameViewEditorWindow, IEditorWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(GameViewEditorWindow) : IEditorWindow(counters, GameViewEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor