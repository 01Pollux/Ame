#include <EditorPlugin/Windows/GameView/GameViewEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> GameViewEditorWindow::Create()
    {
        return { ObjectAllocator<GameViewEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor