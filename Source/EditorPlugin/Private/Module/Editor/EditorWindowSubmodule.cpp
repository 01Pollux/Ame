#include <Module/Editor/EditorWindowSubmodule.hpp>
#include <EditorPlugin/EditorWindowManager.hpp>

namespace Ame
{
    EditorWindowSubmodule::EditorWindowSubmodule(
        IReferenceCounters*                          counters,
        const Editor::EditorWindowManagerCreateDesc& editorManagerCreateDesc) :
        Base(counters, IID_EditorWindowSubmodule),
        m_EditorWindowManager(Editor::CreateEditorWindowManager(editorManagerCreateDesc))
    {
    }
} // namespace Ame