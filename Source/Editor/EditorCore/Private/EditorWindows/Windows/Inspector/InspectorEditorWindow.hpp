#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class InspectorEditorWindow : public IEditorWindow
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_InspectorEditorWindow, IEditorWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(InspectorEditorWindow) : IEditorWindow(counters, InspectorEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor