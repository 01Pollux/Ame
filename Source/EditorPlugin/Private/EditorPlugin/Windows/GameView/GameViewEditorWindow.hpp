#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class GameViewEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create();

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_GameViewEditorWindow, IID_BaseEditorWindow, Base);

    public:
        GameViewEditorWindow(
            IReferenceCounters* counter) :
            Base(counter, "Scene$Game View")
        {
        }
    };
} // namespace Ame::Editor