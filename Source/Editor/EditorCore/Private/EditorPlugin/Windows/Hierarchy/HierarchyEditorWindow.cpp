#include <EditorPlugin/Windows/Hierarchy/HierarchyEditorWindow.hpp>
#include <EditorPlugin/Windows/Hierarchy/EntityHierarchyDisplayer.hpp>
#include <EditorCore/ModuleRegistryUtils.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>
#include <Log/Logger.hpp>

namespace Ame::Editor
{
    HierarchyEditorWindow::HierarchyEditorWindow(IReferenceCounters* counters) :
        Base(counters, HierarchyEditorWindowPath), m_World(ModuleUtils::GetWorld())
    {
    }

    void HierarchyEditorWindow::OnDrawVisible()
    {
        imcxx::window window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
        if (!window)
        {
            return;
        }

        if (m_EntityToRename && !m_EntityToRename->is_alive())
        {
            m_EntityToRename = {};
        }

        auto rootScene = Ecs::RuntimeScene::GetCurrent(m_World);

        // First we need to display the children of root entity.
        EntityHierarchyDisplayer displayEntities(rootScene, m_EntityToRename);
        if (rootScene)
        {
            for (auto& entity : rootScene->GetRoot().GetChildren())
            {
                displayEntities.DisplayEdit(entity, true);
            }

            // Display popup for creating entities in the root when right clicked in the void.
            if (imcxx::popup voidPopup{ imcxx::popup::context_window{}, nullptr,
                                        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems })
            {
                displayEntities.DisplayAdd();
            }
        }
    }
} // namespace Ame::Editor