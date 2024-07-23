#pragma once

#include <EditorPlugin/EditorWindow.hpp>

namespace Ame::Editor
{
    class ContentBrowserEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        static Ptr<IEditorWindow> Create();

        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_ContentBrowserEditorWindow, IID_BaseEditorWindow, Base);

    public:
        ContentBrowserEditorWindow(
            IReferenceCounters* counter) :
            Base(counter, c_ContentBrowserEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor