#pragma once

#include <Core/Interface.hpp>

namespace Ame::Editor
{
    class IProject;
} // namespace Ame::Editor

namespace Ame::Interfaces
{
    // {0150C09D-7439-4D7B-A6C0-A398004E3E8F}
    inline constexpr UId IID_ProjectManager{
        0x1fc4a94c, 0x43d8, 0x4000, { 0xa5, 0xcd, 0x4d, 0x54, 0x12, 0xb0, 0xd0, 0x8 }
    };

    class IEditorProjectManager : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        virtual Editor::IProject*               CreateProject(const String& name, const String& path) = 0;
        virtual Editor::IProject*               OpenProject(const String& path)                       = 0;
        [[nodiscard]] virtual Editor::IProject* GetCurrentProject()                                   = 0;
    };
} // namespace Ame::Interfaces