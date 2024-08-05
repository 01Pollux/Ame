#pragma once

#include <map>
#include <unordered_set>
#include <Core/String.hpp>

#include <EditorPlugin/EditorWindowManager.hpp>
#include <EditorPlugin/EditorWindowManagerDesc.hpp>

#include <Module/Graphics/RendererSubmodule.hpp>

#include <Window/Window.hpp>
#include <Window/DesktopWindow.hpp>

namespace Ame::Editor
{
    class EditorWindowManagerImpl final : public BaseObject<IEditorWindowManager>
    {
    public:
        using Base = BaseObject<IEditorWindowManager>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_EditorWindowManager, Base);

    public:
        EditorWindowManagerImpl(
            IReferenceCounters*                  counters,
            ModuleRegistry&                      registry,
            RhiModule*                           rhiModule,
            GraphicsModule*                      graphicsModule,
            const EditorWindowManagerCreateDesc& createDesc);

    public:
        ModuleRegistry& GetRegistry() override;

        void ResetDefaultWindows() override;

        void AddWindow(
            const EditorWindowCreateDesc& desc) override;

        void RemoveWindow(
            IEditorWindow* window) override;

        void ShowWindow(
            IEditorWindow* window) override;

        void HideWindow(
            IEditorWindow* window) override;

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
        Ref<ModuleRegistry>         m_Registry;
        Ptr<Window::IDesktopWindow> m_DesktopWindow;

        std::map<String, Ptr<IEditorWindow>> m_Windows;
        std::unordered_set<IEditorWindow*>   m_OpenWindows;

        Ame::Signals::ScopedConnection m_OnWindowTitleHitTest;
        Ame::Signals::ScopedConnection m_OnImGuiRender;

        bool m_IsTitlebarHovered = false;
    };

    [[nodiscard]] Ptr<IEditorWindowManager> CreateEditorWindowManager();
} // namespace Ame::Editor