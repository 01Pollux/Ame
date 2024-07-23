#include <EditorPlugin/Windows/Hierachy/HierachyEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> HierachyEditorWindow::Create()
    {
        return { ObjectAllocator<HierachyEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor