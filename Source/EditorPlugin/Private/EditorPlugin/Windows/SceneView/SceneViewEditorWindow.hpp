#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class SceneViewEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create();

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_SceneViewEditorWindow, IID_BaseEditorWindow, Base);

    public:
        SceneViewEditorWindow(
            IReferenceCounters* counter) :
            Base(counter, "Scene$Scene View")
        {
        }
    };
} // namespace Ame::Editor