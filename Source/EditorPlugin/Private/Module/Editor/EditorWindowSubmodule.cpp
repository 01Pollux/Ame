#include <Module/Editor/EditorWindowSubmodule.hpp>
#include <EditorPlugin/EditorWindowManager.hpp>

namespace Ame
{
    EditorWindowSubmodule::EditorWindowSubmodule(IReferenceCounters* counters, ModuleRegistry& registry,
                                                 RhiModule* rhiModule, GraphicsModule* graphicsModule,
                                                 const EditorModuleConfig& config) :
        Base(counters, IID_EditorWindowSubmodule),
        m_EditorWindowManager(
            Editor::CreateEditorWindowManager(registry, rhiModule, graphicsModule, config.EditorManagerDesc))
    {
    }
} // namespace Ame