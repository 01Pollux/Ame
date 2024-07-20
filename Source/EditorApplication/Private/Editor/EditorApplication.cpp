#include <Editor/EditorApplication.hpp>

namespace Ame
{
    EditorApplication::EditorApplication(
        const EditorApplicationConfig& editorConfig) :
        Base(editorConfig.Application)
    {
    }

    void EditorApplication::OnLoad()
    {
        Base::OnLoad();
    }

    void EditorApplication::OnInitialize()
    {
        Base::OnInitialize();
    }

    void EditorApplication::OnShutdown()
    {
        Base::OnShutdown();
    }

    void EditorApplication::OnUnload()
    {
        Base::OnUnload();
    }
} // namespace Ame
