#pragma once

#include <Interfaces/EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class HierarchyEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_HierarchyEditorWindow, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(HierarchyEditorWindow) : Base(counters, HierarchyEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor