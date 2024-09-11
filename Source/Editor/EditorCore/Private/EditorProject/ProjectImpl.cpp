#include <EditorProject/ProjectImpl.hpp>
#include <EditorPlugin/ProjectProperties.hpp>
#include <EditorPlugin/ProjectRequest.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <fstream>

namespace Ame::Editor
{
    Ptr<IProject> ProjectImpl::Create(const String& projectRootPath, const String& projectName)
    {
        auto project = AmeCreate(ProjectImpl, projectRootPath);
        std::filesystem::create_directories(projectRootPath);

        project->SetProperty(String{ ProjectPropNames::Name }, PropertyTree{ projectName });
        project->SetProperty(String{ ProjectPropNames::Version }, PropertyTree{ "1.0.0" });
        project->Save();

        return project;
    }

    Ptr<IProject> ProjectImpl::Open(const String& projectPath)
    {
        auto project = AmeCreate(ProjectImpl, projectPath);

        std::ifstream file(projectPath);
        boost::property_tree::read_xml(file, project->m_Properties);
        project->m_EventListener.OnReload.Invoke();

        return project;
    }

    //

    void ProjectImpl::Save()
    {
        m_EventListener.OnSave.Invoke();
        std::ofstream file(ResolvePath(ProjectDataPath::Solution));
        boost::property_tree::write_xml(file, m_Properties);
    }

    void ProjectImpl::Reload()
    {
        std::ifstream file(ResolvePath(ProjectDataPath::Solution));
        boost::property_tree::read_xml(file, m_Properties);
        m_EventListener.OnReload.Invoke();
    }

    void ProjectImpl::Close()
    {
        m_EventListener.OnClose.Invoke();
    }

    auto ProjectImpl::ResolvePath(ProjectDataPath type) const -> std::filesystem::path
    {
        auto path = m_SolutionRootPath;
        switch (type)
        {
        case ProjectDataPath::SolutionRoot:
            break;
        case ProjectDataPath::Solution:
            path /= GetProperty(String{ ProjectPropNames::Name })->get_value<String>() + ".ame";
            break;
        case ProjectDataPath::ProjectRoot:
            path /= ".ame";
            break;
        case ProjectDataPath::Cache:
            path /= ".ame/Cache";
            break;
        case ProjectDataPath::ProjectData:
            path /= ".ame/ProjectData";
            break;
        default:
            std::unreachable();
        }
        return path;
    }

    auto ProjectImpl::ResolvePath(ProjectDataPath type, const String& path) const -> std::filesystem::path
    {
        return ResolvePath(type) / path;
    }

    auto ProjectImpl::GetProperty(const PropertyPath& name) const -> const PropertyTree*
    {
        auto prop = m_Properties.get_child_optional(name);
        return prop ? &prop.value() : nullptr;
    }

    void ProjectImpl::SetProperty(const PropertyPath& name, const PropertyTree& value)
    {
        m_Properties.put_child(name, value);
    }

    auto ProjectImpl::GetEventListener() -> ProjectEventListener&
    {
        return m_EventListener;
    }
} // namespace Ame::Editor