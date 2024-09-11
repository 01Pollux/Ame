#include <filesystem>
#include <EditorProject/ProjectManagerImpl.hpp>
#include <EditorProject/ProjectImpl.hpp>

namespace Ame::Editor
{
    IProject* ProjectManagerImpl::CreateProject(const String& name, const String& path)
    {
        if (std::filesystem::exists(path))
        {
            throw std::runtime_error("Project already exists at the specified path");
        }

        m_CurrentProject = ProjectImpl::Create(path, name);
        return m_CurrentProject;
    }

    IProject* ProjectManagerImpl::OpenProject(const String& path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error("Project does not exist at the specified path");
        }

        m_CurrentProject = ProjectImpl::Open(path);
        return m_CurrentProject;
    }

    IProject* ProjectManagerImpl::GetCurrentProject()
    {
        return m_CurrentProject;
    }
} // namespace Ame::Editor