#include <EditorWindows/EditorWindowManagerImpl.hpp>
#include <EditorPlugin/EditorCore.hpp>

#include <Window/WindowEventListener.hpp>

#include <EditorWindows/Windows/Console/ConsoleEditorWindow.hpp>
#include <EditorWindows/Windows/ContentBrowser/ContentBrowserEditorWindow.hpp>
#include <EditorWindows/Windows/GameView/GameViewEditorWindow.hpp>
#include <EditorWindows/Windows/Hierarchy/HierarchyEditorWindow.hpp>
#include <EditorWindows/Windows/Inspector/InspectorEditorWindow.hpp>
#include <EditorWindows/Windows/SceneView/SceneViewEditorWindow.hpp>

#include <Plugin/ModuleRegistry.hpp>
#include <Interfaces/Rhi/RhiDevice.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

#include <Log/Logger.hpp>

namespace Ame::Editor
{
    EditorWindowManagerImpl::EditorWindowManagerImpl(IReferenceCounters* counters) : IEditorWindowManager(counters)
    {
        Ptr<Interfaces::IRhiDevice> rhiDevice;
        Ptr<Interfaces::IRenderer>  renderer;

        s_ModuleRegistry->RequestInterface(s_ThisPlugin, Interfaces::IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        s_ModuleRegistry->RequestInterface(s_ThisPlugin, Interfaces::IID_Renderer, renderer.DblPtr<IObject>());

        rhiDevice->QueryInterface(Window::IID_Window, m_Window.DblPtr<IObject>());
        m_OnWindowTitleHitTest = m_Window->GetEventListener().OnWindowTitleHitTest.Connect(
            [this](const Math::Vector2I&) { return m_IsTitlebarHovered; });
        m_OnImGuiRender = renderer->OnImGuiRender.Connect(std::bind(&EditorWindowManagerImpl::Render, this));

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
                 AmeCreateAs(ConsoleEditorWindow, IEditorWindow),
                 AmeCreateAs(ContentBrowserEditorWindow,  IEditorWindow),
                 AmeCreateAs(GameViewEditorWindow,  IEditorWindow),
                 AmeCreateAs(HierarchyEditorWindow, IEditorWindow),
                 AmeCreateAs(InspectorEditorWindow, IEditorWindow),
                 AmeCreateAs(SceneViewEditorWindow, IEditorWindow) })
        // clang-format on
        {
            windowDesc.Window = window;
            AddWindow(windowDesc);
        }
    }

    void EditorWindowManagerImpl::AddWindow(const EditorWindowCreateDesc& desc)
    {
        const auto& path = desc.Window->GetFullPath();
        if (m_Windows.contains(path))
        {
            AME_LOG_WARNING(std::format("Window with path '{}' already exists", path));
            return;
        }
        m_Windows.emplace(path, desc.Window);
        if (desc.DefaultVisible)
        {
            OpenWindow(desc.Window);
        }
    }

    void EditorWindowManagerImpl::RemoveWindow(IEditorWindow* window)
    {
        auto iter = m_Windows.find(window->GetFullPath());
        if (iter == m_Windows.end())
        {
            AME_LOG_WARNING(std::format("Window with path '{}' not found", window->GetFullPath()));
            return;
        }
        if (IsWindowOpen(window))
        {
            CloseWindow(window);
        }
        m_Windows.erase(iter);
    }

    void EditorWindowManagerImpl::ShowWindow(IEditorWindow* window)
    {
        if (!m_Windows.contains(window->GetFullPath())) [[unlikely]]
        {
            AME_LOG_WARNING(std::format("Window with path '{}' not found", window->GetFullPath()));
            return;
        }
        OpenWindow(window);
    }

    void EditorWindowManagerImpl::HideWindow(IEditorWindow* window)
    {
        if (!m_Windows.contains(window->GetFullPath())) [[unlikely]]
        {
            AME_LOG_WARNING(std::format("Window with path '{}' not found", window->GetFullPath()));
            return;
        }
        CloseWindow(window);
    }

    //

    void EditorWindowManagerImpl::CloseWindow(IEditorWindow* window)
    {
        window->OnHide();
        m_OpenWindows.erase(window);
    }

    void EditorWindowManagerImpl::OpenWindow(IEditorWindow* window)
    {
        window->OnShow();
        m_OpenWindows.insert(window);
    }

    bool EditorWindowManagerImpl::IsWindowOpen(IEditorWindow* window) const
    {
        return m_OpenWindows.contains(window);
    }
} // namespace Ame::Editor