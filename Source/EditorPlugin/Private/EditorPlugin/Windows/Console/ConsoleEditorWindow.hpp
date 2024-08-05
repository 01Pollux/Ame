#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class ConsoleEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create(
            ModuleRegistry& registry);

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_ConsoleEditorWindow, IID_BaseEditorWindow, Base);

    public:
        ConsoleEditorWindow(
            IReferenceCounters* counter,
            ModuleRegistry&     registry) :
            Base(counter, registry, c_ConsoleEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor