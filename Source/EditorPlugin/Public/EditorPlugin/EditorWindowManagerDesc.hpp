#pragma once

#include <Core/Interface.hpp>

namespace Ame::Editor
{
    class IEditorWindow;

    struct EditorWindowCreateDesc
    {
        IEditorWindow* Window         = nullptr;
        bool           DefaultVisible = true;
    };

    struct EditorWindowManagerCreateDesc
    {
    };
} // namespace Ame::Editor