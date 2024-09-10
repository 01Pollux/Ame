#pragma once

#include <EditorPlugin/EditorWindow.hpp>
#include <EditorPlugin/StandardWindows.hpp>

#include <Ecs/Entity.hpp>

namespace Ame::Editor
{
    class HierarchyEditorWindow : public BaseObject<IEditorWindow>
    {
    public:
        using Base = BaseObject<IEditorWindow>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_HierarchyEditorWindow, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(HierarchyEditorWindow);

    public:
        void OnDrawVisible() override;

    private:
        Ptr<Ecs::World> m_World;
        Ecs::Entity     m_EntityToRename;
    };
} // namespace Ame::Editor