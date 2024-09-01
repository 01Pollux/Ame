#include <EditorPlugin/EditorWindowManagerImpl.hpp>
#include <Interfaces/EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

#include <ImGuiUtils/Utils.hpp>
#include <ImGuiUtils/WindowUtils.hpp>
#include <imgui_internal.h>

namespace Ame::Editor
{
    void EditorWindowManagerImpl::Render()
    {
        if (BeginEditorSpace())
        {
            RenderMenuBar();
            RenderActiveWindows();
            EndEditorSpace();
        }
    }

    bool EditorWindowManagerImpl::BeginEditorSpace()
    {
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        constexpr ImGuiWindowFlags c_EditorWindowFlags =
            ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImVec2 paddedSize(std::ceilf(viewport->WorkSize.x), std::ceilf(viewport->WorkSize.y));
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(paddedSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

        bool editorOpen;
        {
            imcxx::shared_style overrideStyle(
                ImGuiStyleVar_WindowPadding, ImVec2{}, ImGuiStyleVar_WindowBorderSize, 0.0f);
            imcxx::shared_color overrideBg(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

            editorOpen = ImGui::Begin("Neon Editor", nullptr, c_EditorWindowFlags);
        }

        // Submit the DockSpace
        ImGuiID dockerspaceId = ImGui::GetID("MainDockspace##NEON");
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) && !ImGui::DockBuilderGetNode(dockerspaceId))
        {
            // Clear out existing layout
            ImGui::DockBuilderRemoveNode(dockerspaceId);

            // Add empty node
            ImGui::DockBuilderAddNode(dockerspaceId, ImGuiDockNodeFlags_DockSpace);

            // Main node should cover entire window
            ImGui::DockBuilderSetNodeSize(dockerspaceId, ImGui::GetWindowViewport()->Size);

            // Build dock layout
            ImGuiID center = dockerspaceId;

            ImGuiID topCenter = ImGui::DockBuilderSplitNode(center, ImGuiDir_Up, .75f, nullptr, &center);
            ImGuiID bottom    = center;

            ImGuiID left  = ImGui::DockBuilderSplitNode(topCenter, ImGuiDir_Left, .25f, nullptr, &topCenter);
            ImGuiID right = ImGui::DockBuilderSplitNode(topCenter, ImGuiDir_Right, .25f, nullptr, &topCenter);

            /**
             *  We want to create a dock space like this:
             * __________________________________
             * |       |_B_|_C_|_______|        |
             * |       |               |        |
             * |   A   |               |   D    |
             * |       |      B/C      |        |
             * |       |               |        |
             * |_______|_______________|________|
             * |_E_|_F_|________________________|
             * |                                |
             * |                E/F             |
             * |________________________________|
             *
             * A = Hierarchy view
             * B = Scene view
             * C = Game view
             * D = Inspector view
             * E = Console view
             * F = Content browser
             */

            ImGui::DockBuilderDockWindow(HierarchyEditorWindowPath, left);
            ImGui::DockBuilderDockWindow(SceneViewEditorWindowPath, topCenter);
            ImGui::DockBuilderDockWindow(GameViewEditorWindowPath, topCenter);
            ImGui::DockBuilderDockWindow(InspectorEditorWindowPath, right);
            ImGui::DockBuilderDockWindow(ConsoleEditorWindowPath, bottom);
            ImGui::DockBuilderDockWindow(ContentBrowserEditorWindowPath, bottom);

            ImGui::DockBuilderFinish(dockerspaceId);
        }

        constexpr ImGuiDockNodeFlags c_MainEditorWindowDockSpaceFlags =
            ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        ImGui::DockSpace(dockerspaceId, {}, c_MainEditorWindowDockSpaceFlags);
        return editorOpen;
    }

    void EditorWindowManagerImpl::RenderMenuBar()
    {
        bool displayMenuBar;
        {
            ImVec2 framePadding = ImGui::GetStyle().FramePadding;
            framePadding.y *= 2.8f;

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);
            displayMenuBar = ImGui::BeginMainMenuBar();
        }

        m_IsTitlebarHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

        if (!displayMenuBar)
        {
            ImGui::PopStyleVar();
            return;
        }

        auto curPos = ImGui::GetCursorPos();
        // Render the titlebar buttons if we have a desktop window
        if (m_DesktopWindow)
        {
            auto titlebarHeight = ImGui::GetCurrentWindow()->MenuBarHeight;
            auto buttonSize     = ImVec2(titlebarHeight * 1.4f, titlebarHeight);

            imcxx::shared_style OverrideSpacing(ImGuiStyleVar_ItemSpacing, ImVec2{});
            imcxx::shared_color OverrideColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_MenuBarBg),
                                              ImGuiCol_ButtonActive, ImGui::GetColorU32(ImGuiCol_ScrollbarGrabActive),
                                              ImGuiCol_ButtonHovered,
                                              ImGui::GetColorU32(ImGuiCol_ScrollbarGrabHovered));

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonSize.x * 3);

            if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE, buttonSize))
            {
                m_DesktopWindow->Minimize();
            }

            bool IsMaximized = m_DesktopWindow->IsMaximized();
            if (ImGui::Button(IsMaximized ? ICON_FA_WINDOW_RESTORE : ICON_FA_WINDOW_MAXIMIZE, buttonSize))
            {
                if (IsMaximized)
                {
                    m_DesktopWindow->Restore();
                }
                else
                {
                    m_DesktopWindow->Maximize();
                }
            }

            if (ImGui::Button(ICON_FA_TIMES, buttonSize))
            {
                m_DesktopWindow->Close();
            }
        }
        ImGui::SetCursorPos(curPos);

        if (imcxx::menubar_item fileMenu{ "File" })
        {
            if (imcxx::menuitem_entry{ "New Scene", "Ctrl+N" })
            {
            }

            if (imcxx::menuitem_entry{ "Open Scene...", "Ctrl+O" })
            {
            }

            if (imcxx::menuitem_entry{ "Save Scene", "Ctrl+S" })
            {
            }

            if (imcxx::menuitem_entry{ "Save Scene As...", "Ctrl+Shift+S" })
            {
            }

            ImGui::Separator();

            if (imcxx::menuitem_entry{ "Exit", "Alt+F4" })
            {
            }
        }

        if (imcxx::menubar_item editMenu{ "Edit" })
        {
            if (imcxx::menuitem_entry{ "Undo", "Ctrl+Z" })
            {
            }

            if (imcxx::menuitem_entry{ "Redo", "Ctrl+Y" })
            {
            }

            ImGui::Separator();

            if (imcxx::menuitem_entry{ "Cut", "Ctrl+X" })
            {
            }

            if (imcxx::menuitem_entry{ "Copy", "Ctrl+C" })
            {
            }

            if (imcxx::menuitem_entry{ "Paste", "Ctrl+V" })
            {
            }

            //

            if (imcxx::menuitem_entry{ "Delete", "Del" })
            {
            }
        }

        if (imcxx::menubar_item viewMenu{ "Tools" })
        {
            for (auto& [group, window] : m_Windows)
            {
                bool isOpen = IsWindowOpen(window);
                if (imcxx::menuitem_entry{ group.c_str(), nullptr, isOpen })
                {
                    if (isOpen)
                    {
                        CloseWindow(window);
                    }
                    else
                    {
                        OpenWindow(window);
                    }
                }
            }
        }

        for (auto& window : m_OpenWindows)
        {
            window->OnToolbarDraw();
        }

        m_IsTitlebarHovered &= !ImGui::IsAnyItemHovered();

        ImGui::PopStyleVar();
        ImGui::EndMainMenuBar();
    }

    void EditorWindowManagerImpl::RenderActiveWindows()
    {
        for (auto& window : m_OpenWindows)
        {
            window->OnDrawVisible();
        }
    }

    void EditorWindowManagerImpl::EndEditorSpace()
    {
        ImGui::End();
    }
} // namespace Ame::Editor