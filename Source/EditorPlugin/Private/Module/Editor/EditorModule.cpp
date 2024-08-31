#include <Module/Editor/EditorModule.hpp>

namespace Ame
{
    EditorModule::EditorModule(IReferenceCounters* counters, Dependencies dependencies,
                               const EditorModuleConfig& config) :
        Base(counters, IID_EditorModule),
        m_EditorWindowSubmodule(ObjectAllocator<EditorWindowSubmodule>()(
            *dependencies.Registry, dependencies.RhiMod, dependencies.GfxMod, config))
    {
    }
} // namespace Ame