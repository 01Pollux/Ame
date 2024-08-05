#include <EditorPlugin/Windows/GameView/GameViewEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> GameViewEditorWindow::Create(
        ModuleRegistry& registry)
    {
        return { ObjectAllocator<GameViewEditorWindow>()(registry), IID_BaseEditorWindow };
    }

    void GameViewEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor