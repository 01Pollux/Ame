#include <EditorPlugin/Windows/SceneView/SceneViewEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> SceneViewEditorWindow::Create()
    {
        return { ObjectAllocator<SceneViewEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor