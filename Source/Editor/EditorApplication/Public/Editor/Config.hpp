#pragma once

#include <Application/Application.hpp>

namespace Ame
{
    struct EditorApplicationConfig
    {
        ApplicationConfig Application;

        /// <summary>
        /// Parse from command line arguments, returns false if parsing failed or help was requested.
        /// </summary>
        [[nodiscard]] bool Parse(int argc, char** argv);
    };
} // namespace Ame
