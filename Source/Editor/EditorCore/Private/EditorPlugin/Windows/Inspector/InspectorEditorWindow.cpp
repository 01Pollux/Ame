#include <EditorPlugin/Windows/Inspector/InspectorEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    void InspectorEditorWindow::OnDrawVisible()
    {
        imcxx::window window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor