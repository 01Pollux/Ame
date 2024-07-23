#include <EditorPlugin/Windows/Hierarchy/HierarchyEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> HierarchyEditorWindow::Create()
    {
        return { ObjectAllocator<HierarchyEditorWindow>()(), IID_BaseEditorWindow };
    }

    void HierarchyEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor