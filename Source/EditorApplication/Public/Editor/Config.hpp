#pragma once

#include <Application/Application.hpp>
#include <Module/Editor/Config.hpp>

namespace Ame
{
    struct EditorApplicationConfig
    {
        ApplicationConfig  Application;
        EditorModuleConfig EditorConfig;
    };
} // namespace Ame
