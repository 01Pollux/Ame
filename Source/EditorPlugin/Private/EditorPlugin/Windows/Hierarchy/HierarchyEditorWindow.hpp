#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class HierarchyEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create(
            ModuleRegistry& registry);

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_HierarchyEditorWindow, IID_BaseEditorWindow, Base);

    public:
        HierarchyEditorWindow(
            IReferenceCounters* counter,
            ModuleRegistry&     registry) :
            Base(counter, registry, c_HierarchyEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor