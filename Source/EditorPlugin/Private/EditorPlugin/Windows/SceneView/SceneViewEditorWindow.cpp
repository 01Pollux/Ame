#include <EditorPlugin/Windows/SceneView/SceneViewEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> SceneViewEditorWindow::Create()
    {
        return { ObjectAllocator<SceneViewEditorWindow>()(), IID_BaseEditorWindow };
    }

    void SceneViewEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor