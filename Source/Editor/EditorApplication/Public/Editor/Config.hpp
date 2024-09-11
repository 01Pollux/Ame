#pragma once

#include <Application/Application.hpp>

namespace Ame
{
    /// <summary>
    /// Project configuration for the editor application.
    /// If the ProjectName is not empty, the editor will try to create a new project with the given name and path.
    /// </summary>
    struct EditorApplicationProjectConfig
    {
        String ProjectName;
        String ProjectPath;

        [[nodiscard]] bool IsValid() const
        {
            return !ProjectPath.empty();
        }

        [[nodiscard]] bool ShouldCreateNewProject() const
        {
            return !ProjectName.empty();
        }
    };

    struct EditorApplicationConfig
    {
        ApplicationConfig Application;

        UniquePtr<EditorApplicationProjectConfig> ProjectConfig = std::make_unique<EditorApplicationProjectConfig>();

        /// <summary>
        /// Parse from command line arguments, returns false if parsing failed or help was requested.
        /// </summary>
        [[nodiscard]] bool Parse(int argc, char** argv);

        [[nodiscard]] bool IsValid() const
        {
            return ProjectConfig->IsValid();
        }
    };
} // namespace Ame
