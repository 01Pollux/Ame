#pragma once

#include <EditorPlugin/Project.hpp>

namespace Ame::Editor
{
    class ProjectImpl : public BaseObject<IProject>
    {
    public:
        using Base = BaseObject<IProject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Project, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(ProjectImpl, const String& solutionPath);

    public:
        auto GetName() const -> const String& override
        {
            return m_Name;
        }
        void SetName(String name) override;

        void Save() override;
        void Reload() override;
        void Close() override;

        auto ResolvePath(ProjectDataPath type) const -> std::filesystem::path override;
        auto ResolvePath(ProjectDataPath type, const String& path) const -> std::filesystem::path override;

        auto GetProperties() -> boost::property_tree::ptree& override;
        auto GetProperties() const -> const boost::property_tree::ptree& override;

        auto GetEventListener() -> ProjectEventListener& override;

    private:
        std::filesystem::path m_SolutionRootPath;
        String                m_Name;
    };
} // namespace Ame::Editor