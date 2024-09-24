#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

#include <EditorWindows/Windows/Console/ConsoleLogStream.hpp>

namespace Ame::Editor
{
    class ConsoleEditorWindow : public IEditorWindow
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ConsoleEditorWindow, IEditorWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(ConsoleEditorWindow);

    public:
        ~ConsoleEditorWindow() override;

    public:
        void OnDrawVisible() override;

    private:
        UniquePtr<ConsoleLogStream> m_LogStream;
        bool                        m_AutoScroll;
    };
} // namespace Ame::Editor