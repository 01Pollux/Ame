#pragma once

#include <EditorPlugin/Project.hpp>
#include <EditorPlugin/ProjectEventListener.hpp>

namespace Ame::Editor
{
    class ProjectImpl : public BaseObject<IProject>
    {
    public:
        using Base = BaseObject<IProject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Project, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(ProjectImpl, const String& rootPath) : Base(counters), m_SolutionRootPath(rootPath)
        {
        }

    public:
        void Create(const String& projectName);
        void Open();

    public:
        void Save() override;
        void Reload() override;
        void Close() override;

        auto ResolvePath(ProjectDataPath type) const -> std::filesystem::path override;
        auto ResolvePath(ProjectDataPath type, const String& path) const -> std::filesystem::path override;

        auto GetProperty(const PropertyPath& name) const -> const PropertyTree* override;
        void SetProperty(const PropertyPath& name, const PropertyTree& value);

        auto GetEventListener() -> ProjectEventListener& override;

    private:
        boost::property_tree::ptree m_Properties;
        std::filesystem::path       m_SolutionRootPath;
        ProjectEventListener        m_EventListener;
    };
} // namespace Ame::Editor