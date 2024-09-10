#pragma once

#include <Editor/EditorApplication.hpp>

namespace Ame
{
    [[nodiscard]] EditorApplicationConfig ParseEditorConfig(int argc, char** argv);
}