#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class GameViewEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create(
            ModuleRegistry& registry);

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_GameViewEditorWindow, IID_BaseEditorWindow, Base);

    public:
        GameViewEditorWindow(
            IReferenceCounters* counter,
            ModuleRegistry&     registry) :
            Base(counter, registry, c_GameViewEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor