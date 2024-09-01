#include <EditorPlugin/Windows/ContentBrowser/ContentBrowserEditorWindow.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>

namespace Ame::Editor
{
    void ContentBrowserEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
    }
} // namespace Ame::Editor