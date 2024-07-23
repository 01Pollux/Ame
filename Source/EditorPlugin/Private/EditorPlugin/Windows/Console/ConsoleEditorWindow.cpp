#include <EditorPlugin/Windows/Console/ConsoleEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> ConsoleEditorWindow::Create()
    {
        return { ObjectAllocator<ConsoleEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor