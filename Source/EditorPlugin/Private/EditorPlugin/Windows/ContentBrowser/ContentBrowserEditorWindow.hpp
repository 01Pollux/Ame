#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class ContentBrowserEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create(
            ModuleRegistry& registry);

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_ContentBrowserEditorWindow, IID_BaseEditorWindow, Base);

    public:
        ContentBrowserEditorWindow(
            IReferenceCounters* counter,
            ModuleRegistry&     registry) :
            Base(counter, registry, c_ContentBrowserEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor