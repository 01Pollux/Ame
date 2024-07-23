#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class InspectorEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create();

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_InspectorEditorWindow, IID_BaseEditorWindow, Base);

    public:
        InspectorEditorWindow(
            IReferenceCounters* counter) :
            Base(counter, c_InspectorEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor