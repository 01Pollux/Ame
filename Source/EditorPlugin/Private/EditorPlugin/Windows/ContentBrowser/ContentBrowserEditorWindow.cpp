#include <EditorPlugin/Windows/ContentBrowser/ContentBrowserEditorWindow.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> ContentBrowserEditorWindow::Create()
    {
        return { ObjectAllocator<ContentBrowserEditorWindow>()(), IID_BaseEditorWindow };
    }
} // namespace Ame::Editor