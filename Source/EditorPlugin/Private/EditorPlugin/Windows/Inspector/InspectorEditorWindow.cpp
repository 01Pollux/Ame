#include <EditorPlugin/Windows/Inspector/InspectorEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> InspectorEditorWindow::Create()
    {
        return { ObjectAllocator<InspectorEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor