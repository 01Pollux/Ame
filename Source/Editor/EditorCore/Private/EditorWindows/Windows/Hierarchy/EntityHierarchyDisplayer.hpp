#pragma once

#include <functional>
#include <Ecs/Entity.hpp>
#include <EcsComponent/Scene/RuntimeScene.hpp>

namespace Ame::Editor
{
    class EntityHierarchyDisplayer
    {
    public:
        EntityHierarchyDisplayer(Ecs::RuntimeScene* scene, Ecs::Entity& entityToRename) noexcept;

        EntityHierarchyDisplayer(const EntityHierarchyDisplayer&)            = delete;
        EntityHierarchyDisplayer& operator=(const EntityHierarchyDisplayer&) = delete;

        EntityHierarchyDisplayer(EntityHierarchyDisplayer&&)            = default;
        EntityHierarchyDisplayer& operator=(EntityHierarchyDisplayer&&) = default;

        ~EntityHierarchyDisplayer();

    public:
        void DisplayEdit(const Ecs::Entity& entity, bool editable = true);
        void DisplayAdd(const Ecs::Entity& entity = {});

    private:
        void DisplayEdit(const Ecs::Entity& selectedEntity, const Ecs::Entity& parentEntity, bool editable);

    private:
        void PushTask(std::move_only_function<void()> task);

    private:
        Ecs::Entity HelperCreateEntity(const Ecs::Entity& parentEntity);
        Ecs::Entity HelperCreateEntityWithTransform(const Ecs::Entity& parentEntity);

    private:
        static inline char m_RenameBuffer[256] = "";

        Ecs::RuntimeScene*              m_RuntimeScene;
        Ecs::Entity&                    m_EntityToRename;
        std::move_only_function<void()> m_DeferredTask;
    };
} // namespace Ame::Editor