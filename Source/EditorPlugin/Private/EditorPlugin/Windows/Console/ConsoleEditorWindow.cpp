#include <EditorPlugin/Windows/Console/ConsoleEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> ConsoleEditorWindow::Create()
    {
        return { ObjectAllocator<ConsoleEditorWindow>()(), IID_BaseEditorWindow };
    }

    void ConsoleEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor