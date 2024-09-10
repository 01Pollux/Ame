#pragma once

#include <Core/Interface.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Ame::Editor
{
    class ProjectEventListener;

    enum class ProjectDataPath : uint8_t
    {
        SolutionRoot, // "<path>"
        Solution,     // "<path>/<project_name>.ame" : Contains informations project's settings
        ProjectRoot,  // "<path>/.ame"
        Cache,        // "<path>/.ame/Cache" : Contains all cache data for faster loading and lookups
        ProjectData,  // "<path>/.ame/ProjectData" : Contains all project's related data, such as scenes, assets, etc.
    };

    class IProject : public IObject
    {
    public:
        virtual auto GetName() const -> const String& = 0;
        virtual void SetName(const String& name)      = 0;

        virtual void Save()   = 0;
        virtual void Reload() = 0;
        virtual void Close()  = 0;

        virtual auto ResolvePath(ProjectDataPath type) const -> std::filesystem::path                     = 0;
        virtual auto ResolvePath(ProjectDataPath type, const String& path) const -> std::filesystem::path = 0;

        virtual auto GetProperties() -> boost::property_tree::ptree&             = 0;
        virtual auto GetProperties() const -> const boost::property_tree::ptree& = 0;

        virtual auto GetEventListener() -> ProjectEventListener& = 0;
    };
} // namespace Ame::Editor