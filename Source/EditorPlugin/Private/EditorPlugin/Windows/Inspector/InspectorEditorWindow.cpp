#include <EditorPlugin/Windows/Inspector/InspectorEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> InspectorEditorWindow::Create()
    {
        return { ObjectAllocator<InspectorEditorWindow>()(), IID_BaseEditorWindow };
    }

    void InspectorEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor