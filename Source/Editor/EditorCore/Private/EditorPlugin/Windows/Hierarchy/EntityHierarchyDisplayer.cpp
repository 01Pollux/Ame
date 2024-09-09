#include <EditorPlugin/Windows/Hierarchy/EntityHierarchyDisplayer.hpp>

#include <Ecs/EntityUtils.hpp>
#include <EcsComponent/Math/Transform.hpp>

#include <ImGuiUtils/imcxx/all_in_one.hpp>
#include <Log/Logger.hpp>

namespace Ame::Editor
{
    EntityHierarchyDisplayer::EntityHierarchyDisplayer(Ecs::RuntimeScene* scene,
                                                       const Ecs::Entity& entityToRename) noexcept :
        m_RuntimeScene(scene),
        m_EntityToRename(entityToRename)
    {
    }

    EntityHierarchyDisplayer ::~EntityHierarchyDisplayer()
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

    //

    void EntityHierarchyDisplayer::DisplayEdit(const Ecs::Entity& entity, bool editable)
    {
        DisplayEdit({}, entity, editable);
    }

    void EntityHierarchyDisplayer::DisplayAdd(const Ecs::Entity& entity)
    {
        if (imcxx::menuitem_entry{ "Empty" })
        {
            PushTask([entity, this]() mutable { HelperCreateEntity(entity); });
        }

        if (imcxx::menubar_item Menu2D{ "2D" })
        {
            if (imcxx::menubar_item MenuSprite{ "Sprite" })
            {
                if (imcxx::menuitem_entry{ "Triangle" })
                {
                }

                if (imcxx::menuitem_entry{ "Quad" })
                {
                }

                if (imcxx::menuitem_entry{ "Circle" })
                {
                }

                if (imcxx::menuitem_entry{ "Polygon" })
                {
                }

                if (imcxx::menuitem_entry{ "Capsule" })
                {
                }

                if (imcxx::menuitem_entry{ "Hexagon-Flat" })
                {
                }

                if (imcxx::menuitem_entry{ "Hexagon-Pointy" })
                {
                }

                if (imcxx::menuitem_entry{ "9-Sliced" })
                {
                }

                if (imcxx::menuitem_entry{ "Isometric Diamon" })
                {
                }
            }

            if (imcxx::menubar_item MenuPhysics{ "Physics" })
            {
                if (imcxx::menuitem_entry{ "Dynamic sprite" })
                {
                }

                if (imcxx::menuitem_entry{ "Static sprite" })
                {
                }
            }

            if (imcxx::menubar_item MenuTilemap{ "Tilemap" })
            {
                if (imcxx::menuitem_entry{ "Hexagon-Flat" })
                {
                }

                if (imcxx::menuitem_entry{ "Hexagon-Pointy" })
                {
                }

                if (imcxx::menuitem_entry{ "Isometric" })
                {
                }

                if (imcxx::menuitem_entry{ "Rectangular" })
                {
                }
            }
        }

        if (imcxx::menubar_item Menu3D{ "3D" })
        {
            if (imcxx::menuitem_entry{ "Cube" })
            {
                PushTask([entity, this]() mutable { HelperCreateEntityWithTransform(entity); });
            }

            if (imcxx::menuitem_entry{ "Sphere" })
            {
            }

            if (imcxx::menuitem_entry{ "Capsule" })
            {
            }

            if (imcxx::menuitem_entry{ "Cylinder" })
            {
            }

            if (imcxx::menuitem_entry{ "Cone" })
            {
            }

            if (imcxx::menuitem_entry{ "Plane" })
            {
            }

            if (imcxx::menuitem_entry{ "Quad" })
            {
            }
        }

        if (imcxx::menubar_item MenuEffect{ "Effect" })
        {
        }

        if (imcxx::menubar_item MenuLight{ "Light" })
        {
        }

        if (imcxx::menubar_item MenuAudio{ "Audio" })
        {
        }

        if (imcxx::menubar_item MenuUI{ "UI" })
        {
        }

        if (imcxx::menubar_item MenuVolume{ "Volume" })
        {
        }

        if (imcxx::menubar_item MenuCamera{ "Camera" })
        {
            if (imcxx::menuitem_entry{ "Generic" })
            {
            }

            if (imcxx::menuitem_entry{ "Pixel perfect" })
            {
            }
        }
    }

    //

    void EntityHierarchyDisplayer::DisplayEdit(const Ecs::Entity& selectedEntity, const Ecs::Entity& parentEntity,
                                               bool editable)
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
                    m_DeferredTask = [entity = parentEntity]() mutable { entity.Reset(true); };
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
                    PushTask([entity = parentEntity]() mutable
                             { Ecs::EntityUtils::Clone(entity, entity.GetParent()); });
                }

                if (imcxx::menuitem_entry{ "Delete (Recursive)" })
                {
                    PushTask([entity = parentEntity]() mutable { entity.Reset(true); });
                }

                if (imcxx::menuitem_entry{ "Delete" })
                {
                    // Delete only entity, and move children to parent.
                    PushTask([entity = parentEntity]() mutable { entity.Reset(false); });
                }

                if (isDisabled)
                {
                    if (imcxx::menuitem_entry{ "Enable" })
                    {
                        PushTask([parentEntity]() mutable { parentEntity->enable(); });
                    }
                }
                else
                {
                    if (imcxx::menuitem_entry{ "Disable" })
                    {
                        PushTask([parentEntity]() mutable { parentEntity->disable(); });
                    }
                }

                ImGui::Separator();

                DisplayAdd(parentEntity);
            }
        }

        if (hierachyNode)
        {
            for (auto& child : parentEntity.GetChildren())
            {
                DisplayEdit(selectedEntity, child, editable);
            }
        }
    }

    //

    void EntityHierarchyDisplayer::PushTask(std::move_only_function<void()> task)
    {
        m_DeferredTask = std::move(task);
    }

    //

    Ecs::Entity EntityHierarchyDisplayer::HelperCreateEntity(const Ecs::Entity& parentEntity)
    {
        auto entity = m_RuntimeScene->CreateEntity("New Entity");
        if (parentEntity)
        {
            entity.SetParent(parentEntity);
        }
        return entity;
    }

    Ecs::Entity EntityHierarchyDisplayer::HelperCreateEntityWithTransform(const Ecs::Entity& parentEntity)
    {
        auto entity = HelperCreateEntity(parentEntity);
        entity->add<Ecs::TransformComponent>();
        return entity;
    }
} // namespace Ame::Editor