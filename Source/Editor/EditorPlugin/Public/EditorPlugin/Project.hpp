#pragma once

#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame::Editor
{
    // {A6FB5C58-75D9-479C-BAD7-AAD18FFDDFDC}
    inline constexpr UId IID_Project{ 0xa6fb5c58, 0x75d9, 0x479c, { 0xba, 0xd7, 0xaa, 0xd1, 0x8f, 0xfd, 0xdf, 0xdc } };

    struct ProjectEventListener;

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
        using PropertyTree = boost::property_tree::ptree;
        using PropertyPath = boost::property_tree::path;

    public:
        virtual void Save()   = 0;
        virtual void Reload() = 0;
        virtual void Close()  = 0;

        virtual auto ResolvePath(ProjectDataPath type) const -> std::filesystem::path                     = 0;
        virtual auto ResolvePath(ProjectDataPath type, const String& path) const -> std::filesystem::path = 0;

        virtual auto GetProperty(const PropertyPath& name) const -> const PropertyTree* = 0;
        virtual void SetProperty(const PropertyPath& name, const PropertyTree& value)   = 0;

        virtual auto GetEventListener() -> ProjectEventListener& = 0;
    };
} // namespace Ame::Editor