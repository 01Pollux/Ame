#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

namespace Ame::Editor
{
    class ContentBrowserEditorWindow : public IEditorWindow
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ContentBrowserEditorWindow, IEditorWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(ContentBrowserEditorWindow) : IEditorWindow(counters, ContentBrowserEditorWindowPath)
        {
        }

    public:
        void OnDrawVisible() override;
    };
} // namespace Ame::Editor