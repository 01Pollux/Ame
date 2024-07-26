#pragma once

#include <Module/Submodule.hpp>

#include <Module/Editor/Config.hpp>
#include <EditorPlugin/EditorWindowManager.hpp>

namespace Ame
{
    class EditorWindowSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EditorWindowSubmodule, IID_BaseSubmodule, Base,
            m_EditorWindowManager);

    public:
        EditorWindowSubmodule(
            IReferenceCounters*       counters,
            RhiModule*                rhiModule,
            GraphicsModule*           graphicsModule,
            const EditorModuleConfig& config);

    private:
        Ptr<Editor::IEditorWindowManager> m_EditorWindowManager;
    };
} // namespace Ame