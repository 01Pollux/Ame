#include <EditorPlugin/Windows/Hierarchy/HierarchyEditorWindow.hpp>
#include <EditorCore/ModuleRegistryUtils.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>
#include <Log/Logger.hpp>

namespace Ame::Editor
{
    class EntityHierarchyDisplayer
    {
    public:
        EntityHierarchyDisplayer(Ecs::Entity entityToRename) : m_EntityToRename(entityToRename)
        {
        }

        ~EntityHierarchyDisplayer()
        {
            try
            {
                if (m_DeferredTask)
                {
                    m_DeferredTask();
                }
            }
            catch (const std::exception& e)
            {
                AME_LOG_ERROR(std::format("Error while executing deferred task: '{}' for hierarchy editor.", e.what()));
            }
        }

    public:
        void Display(Ecs::Entity parentEntity, bool editable = true);

    private:
        void Display(Ecs::Entity selectedEntity, Ecs::Entity parentEntity, bool editable);

    private:
        static inline char m_RenameBuffer[256] = "";

        Ecs::Entity                     m_EntityToRename;
        std::move_only_function<void()> m_DeferredTask;
    };

    //

    void HierarchyEditorWindow::OnDrawVisible()
    {
        imcxx::window Window{ GetFullPath(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse };
        //if (!Window)
        //{
        //    return;
        //}

        //if (m_EntityToRename && !m_EntityToRename->is_alive())
        //{
        //    m_EntityToRename = {};
        //}

        //// auto RootFilter = Scene::EntityWorld::GetChildrenFilter(EditorEngine::Get()->GetActiveSceneTag()).build();

        //// First we need to display the children of root entity.
        //EntityHierarchyDisplayer displayEntities(m_EntityToRename);
        //for (auto& entity : m_RootEntity.GetChildren())
        //{
        //    displayEntities.Display(entity, true);
        //}

        //// Display popup for creating entities in the root
        //if (imcxx::popup Popup{ imcxx::popup::context_window{}, nullptr,
        //                        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems })
        //{
        //    // DisplayEntityPopup(flecs::entity::null(), DeferredTask);
        //}
    }

    //

    void EntityHierarchyDisplayer::Display(Ecs::Entity selectedEntity, Ecs::Entity parentEntity, bool editable)
    {
        ImGuiTableFlags tableFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                                     ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth;

        // Create filter to check if entity has children.
        auto children = parentEntity.GetChildren();

        // If entity has no children, mark it as leaf.
        if (children.empty())
        {
            tableFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        }

        // If entity is being selected, mark it as selected.
        if (selectedEntity == parentEntity)
        {
            tableFlags |= ImGuiTreeNodeFlags_Framed;
        }

        // If entity is disabled, Make the text gray.
        bool                isDisabled = !editable || !parentEntity->enabled();
        imcxx::shared_color overrideColor;
        if (isDisabled)
        {
            overrideColor.push(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        }

        auto& name = parentEntity.GetName();

        ImGui::SetNextItemAllowOverlap();
        imcxx::tree_node hierachyNode(std::bit_cast<void*>(parentEntity->raw_id()), tableFlags,
                                      m_EntityToRename == parentEntity ? "" : name.c_str());

        bool editingName = false;
        {
            // If we clicked F2 or double click on this entity, set it as the entity to rename.
            if (editable && ImGui::IsItemFocused())
            {
                if (ImGui::IsKeyPressed(ImGuiKey_F2) || ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    m_EntityToRename = parentEntity;
                    strncpy_s(m_RenameBuffer, name.c_str(),
                              name.size() < std::size(m_RenameBuffer) ? name.size() : (std::size(m_RenameBuffer) - 1));
                }
                else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
                {
                    m_DeferredTask = [parentEntity]() mutable { parentEntity.Reset(true); };
                }
            }

            if (m_EntityToRename == parentEntity)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_Escape)) [[unlikely]]
                {
                    m_EntityToRename = {};
                }
                else
                {
                    ImGui::SameLine();
                    editingName = true;

                    // If user pressed enter, or clicked outside the text box, rename the entity.
                    if (ImGui::InputText("##Name", m_RenameBuffer, static_cast<int>(std::size(m_RenameBuffer)),
                                         ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) ||
                        (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
                         ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
                    {
                        m_DeferredTask   = [this, parentEntity]() mutable { parentEntity.SetName(m_RenameBuffer); };
                        m_EntityToRename = {};
                    }
                }
            }
        }

        overrideColor.pop(isDisabled ? 1 : 0);

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            // TODO: Select entity
        }

        if (editable && !editingName) [[likely]]
        {
            if (imcxx::popup EditEntity{ imcxx::popup::context_item{} })
            {
                if (imcxx::menuitem_entry{ "Duplicate" })
                {
                    // TODO
                }

                if (imcxx::menuitem_entry{ "Delete (Recursive)" })
                {
                    m_DeferredTask = [parentEntity]() mutable { parentEntity.Reset(true); };
                }

                if (imcxx::menuitem_entry{ "Delete" })
                {
                    // Delete only entity, and move children to parent.
                    m_DeferredTask = [parentEntity]() mutable { parentEntity.Reset(false); };
                }

                if (isDisabled)
                {
                    if (imcxx::menuitem_entry{ "Enable" })
                    {
                        m_DeferredTask = [parentEntity]() mutable { parentEntity->enable(); };
                    }
                }
                else
                {
                    if (imcxx::menuitem_entry{ "Disable" })
                    {
                        m_DeferredTask = [parentEntity]() mutable { parentEntity->disable(); };
                    }
                }

                ImGui::Separator();

                // DisplayEntityPopup(Entity, DeferredTask);
            }
        }

        if (hierachyNode)
        {
            for (auto& child : parentEntity.GetChildren())
            {
                Display(selectedEntity, child, editable);
            }
        }
    }

    void EntityHierarchyDisplayer::Display(Ecs::Entity parentEntity, bool editable)
    {
        Display({}, std::move(parentEntity), editable);
    }
} // namespace Ame::Editor