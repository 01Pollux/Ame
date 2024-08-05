#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class SceneViewEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create(
            ModuleRegistry& registry);

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_SceneViewEditorWindow, IID_BaseEditorWindow, Base);

    public:
        SceneViewEditorWindow(
            IReferenceCounters* counter,
            ModuleRegistry&     registry) :
            Base(counter, registry, c_SceneViewEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor