#include <EditorPlugin/EditorWindowManagerImpl.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>

#include <Window/DesktopWindow.hpp>
#include <Window/WindowEventListener.hpp>

#include <EditorPlugin/Windows/Console/ConsoleEditorWindow.hpp>
#include <EditorPlugin/Windows/ContentBrowser/ContentBrowserEditorWindow.hpp>
#include <EditorPlugin/Windows/GameView/GameViewEditorWindow.hpp>
#include <EditorPlugin/Windows/Hierarchy/HierarchyEditorWindow.hpp>
#include <EditorPlugin/Windows/Inspector/InspectorEditorWindow.hpp>
#include <EditorPlugin/Windows/SceneView/SceneViewEditorWindow.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Editor
{
    Ptr<IEditorWindowManager> CreateEditorWindowManager(
        RhiModule*                           rhiModule,
        GraphicsModule*                      graphicsModule,
        const EditorWindowManagerCreateDesc& desc)
    {
        return { ObjectAllocator<EditorWindowManagerImpl>()(rhiModule, graphicsModule, desc), IID_EditorWindowManager };
    }

    //

    EditorWindowManagerImpl::EditorWindowManagerImpl(
        IReferenceCounters*                  counters,
        RhiModule*                           rhiModule,
        GraphicsModule*                      graphicsModule,
        const EditorWindowManagerCreateDesc& createDesc) :
        Base(counters)
    {
        Ptr<RendererSubmodule> rendererSubmodule;
        graphicsModule->QueryInterface(IID_RendererSubmodule, rendererSubmodule.DblPtr<IObject>());
        rhiModule->QueryInterface(Window::IID_DesktopWindow, m_DesktopWindow.DblPtr<IObject>());

        m_OnImGuiRender = rendererSubmodule->OnImGuiRender(std::bind(&EditorWindowManagerImpl::Render, this));

        if (m_DesktopWindow)
        {
            m_OnWindowTitleHitTest = m_DesktopWindow->GetEventListener().OnWindowTitleHitTest(
                [this](const Math::Vector2I&)
                {
                    return m_IsTitlebarHovered;
                });
        }

        ResetDefaultWindows();
    }

    //

    void EditorWindowManagerImpl::ResetDefaultWindows()
    {
        m_OpenWindows.clear();
        m_Windows.clear();

        EditorWindowCreateDesc windowDesc;
        // clang-format off
        for (auto& window : {
                 ConsoleEditorWindow::Create() ,
                 ContentBrowserEditorWindow::Create() ,
                 GameViewEditorWindow::Create(),
                 HierarchyEditorWindow::Create(),
                 InspectorEditorWindow::Create(),
                 SceneViewEditorWindow::Create() })
        // clang-format on
        {
            windowDesc.Window = window;
            AddWindow(windowDesc);
        }
    }

    void EditorWindowManagerImpl::AddWindow(
        const EditorWindowCreateDesc& desc)
    {
        const auto& path = desc.Window->GetFullPath();
        if (m_Windows.contains(path))
        {
            Log::Editor().Warning("Window with path '{}' already exists", path.c_str());
            return;
        }
        m_Windows.emplace(path, desc.Window);
        if (desc.DefaultVisible)
        {
            OpenWindow(desc.Window);
        }
    }

    void EditorWindowManagerImpl::RemoveWindow(
        IEditorWindow* window)
    {
        auto iter = m_Windows.find(window->GetFullPath());
        if (iter == m_Windows.end())
        {
            Log::Editor().Warning("Window with path '{}' not found", window->GetFullPath().c_str());
            return;
        }
        if (IsWindowOpen(window))
        {
            CloseWindow(window);
        }
        m_Windows.erase(iter);
    }

    void EditorWindowManagerImpl::ShowWindow(
        IEditorWindow* window)
    {
        if (!m_Windows.contains(window->GetFullPath())) [[unlikely]]
        {
            Log::Editor().Warning("Window with path '{}' not found", window->GetFullPath().c_str());
            return;
        }
        OpenWindow(window);
    }

    void EditorWindowManagerImpl::HideWindow(
        IEditorWindow* window)
    {
        if (!m_Windows.contains(window->GetFullPath())) [[unlikely]]
        {
            Log::Editor().Warning("Window with path '{}' not found", window->GetFullPath().c_str());
            return;
        }
        CloseWindow(window);
    }

    //

    void EditorWindowManagerImpl::CloseWindow(
        IEditorWindow* window)
    {
        window->OnHide();
        m_OpenWindows.erase(window);
    }

    void EditorWindowManagerImpl::OpenWindow(
        IEditorWindow* window)
    {
        window->OnShow();
        m_OpenWindows.insert(window);
    }

    bool EditorWindowManagerImpl::IsWindowOpen(
        IEditorWindow* window) const
    {
        return m_OpenWindows.contains(window);
    }
} // namespace Ame::Editor