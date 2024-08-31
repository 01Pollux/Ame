#include <EditorPlugin/Windows/ContentBrowser/ContentBrowserEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindow> ContentBrowserEditorWindow::Create(ModuleRegistry& registry)
    {
        return { ObjectAllocator<ContentBrowserEditorWindow>()(registry), IID_BaseEditorWindow };
    }

    void ContentBrowserEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor