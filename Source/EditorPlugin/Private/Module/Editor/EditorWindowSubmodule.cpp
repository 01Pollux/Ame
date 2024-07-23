#include <Module/Editor/EditorWindowSubmodule.hpp>
#include <EditorPlugin/EditorWindowManager.hpp>

namespace Ame
{
    EditorWindowSubmodule::EditorWindowSubmodule(
        IReferenceCounters*                          counters,
        RhiModule*                                   rhiModule,
        GraphicsModule*                              graphicsModule,
        const Editor::EditorWindowManagerCreateDesc& editorManagerCreateDesc) :
        Base(counters, IID_EditorWindowSubmodule),
        m_EditorWindowManager(Editor::CreateEditorWindowManager(rhiModule, graphicsModule, editorManagerCreateDesc))
    {
    }
} // namespace Ame