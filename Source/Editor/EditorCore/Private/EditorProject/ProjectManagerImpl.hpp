#pragma once

#include <Interfaces/EditorPlugin/ProjectManager.hpp>
#include <EditorPlugin/Project.hpp>

namespace Ame::Editor
{
    class ProjectManagerImpl : public BaseObject<Interfaces::IEditorProjectManager>
    {
    public:
        using Base = BaseObject<Interfaces::IEditorProjectManager>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(Interfaces::IID_ProjectManager, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(ProjectManagerImpl) : Base(counters)
        {
        }

    public:
        IProject* CreateProject(const String& name, const String& path) override;
        IProject* OpenProject(const String& path) override;
        IProject* GetCurrentProject() override;

    private:
        Ptr<IProject> m_CurrentProject;
    };
} // namespace Ame::Editor