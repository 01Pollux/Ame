#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class ConsoleEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ConsoleEditorWindow, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(ConsoleEditorWindow) : Base(counters, ConsoleEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor