#include <Module/Editor/EditorModule.hpp>

namespace Ame
{
    EditorModule::EditorModule(
        IReferenceCounters*       counters,
        const EditorModuleConfig& config) :
        Base(counters, IID_EditorModule),
        m_EditorWindowSubmodule(ObjectAllocator<EditorWindowSubmodule>()(config.EditorManagerDesc))
    {
    }
} // namespace Ame