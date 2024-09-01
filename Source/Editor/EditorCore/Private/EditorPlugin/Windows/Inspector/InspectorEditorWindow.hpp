#pragma once

#include <Interfaces/EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class InspectorEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_InspectorEditorWindow, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(InspectorEditorWindow) : Base(counters, InspectorEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor