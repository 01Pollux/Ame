#pragma once

#include <map>
#include <unordered_set>
#include <Core/String.hpp>
#include <Core/Signal.hpp>

#include <EditorPlugin/EditorWindowManagerDesc.hpp>
#include <Interfaces/EditorPlugin/EditorWindowManager.hpp>
#include <EditorPlugin/EditorWindow.hpp>

#include <Window/Window.hpp>

namespace Ame::Editor
{
    class EditorWindowManagerImpl final : public Interfaces::IEditorWindowManager
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(Interfaces::IID_EditorWindowManager, IEditorWindowManager);

    private:
        IMPLEMENT_INTERFACE_CTOR(EditorWindowManagerImpl);

    public:
        void ResetDefaultWindows() override;

        void AddWindow(const EditorWindowCreateDesc& desc) override;

        void RemoveWindow(IEditorWindow* window) override;

        void ShowWindow(IEditorWindow* window) override;

        void HideWindow(IEditorWindow* window) override;

    private:
        void Render();

        bool BeginEditorSpace();
        void RenderMenuBar();
        void RenderActiveWindows();
        void EndEditorSpace();

    private:
        void               CloseWindow(IEditorWindow* window);
        void               OpenWindow(IEditorWindow* window);
        [[nodiscard]] bool IsWindowOpen(IEditorWindow* window) const;

    private:
        Ptr<Window::IWindow> m_Window;

        std::map<String, Ptr<IEditorWindow>> m_Windows;
        std::unordered_set<IEditorWindow*>   m_OpenWindows;

        Signals::ScopedConnection m_OnWindowTitleHitTest;
        Signals::ScopedConnection m_OnImGuiRender;

        bool m_IsTitlebarHovered = false;
    };
} // namespace Ame::Editor