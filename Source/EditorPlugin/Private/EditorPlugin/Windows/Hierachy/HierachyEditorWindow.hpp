#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class HierachyEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create();

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_HierachyEditorWindow, IID_BaseEditorWindow, Base);

    public:
        HierachyEditorWindow(
            IReferenceCounters* counter) :
            Base(counter, "Scene$Hierachy")
        {
        }
    };
} // namespace Ame::Editor