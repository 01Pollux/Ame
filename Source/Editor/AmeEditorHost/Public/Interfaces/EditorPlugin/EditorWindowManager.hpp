#pragma once

#include <Core/Interface.hpp>

namespace Ame::Editor
{
    class IEditorWindow;
    struct EditorWindowCreateDesc;
} // namespace Ame::Editor

namespace Ame::Interfaces
{
    // {0150C09D-7439-4D7B-A6C0-A398004E3E8F}
    inline constexpr UId IID_EditorWindowManager{
        0x150c09d, 0x7439, 0x4d7b, { 0xa6, 0xc0, 0xa3, 0x98, 0x0, 0x4e, 0x3e, 0x8f }
    };

    class IEditorWindowManager : public IObject
    {
    public:
        virtual void ResetDefaultWindows() = 0;

        virtual void AddWindow(const Editor::EditorWindowCreateDesc& desc) = 0;

        virtual void RemoveWindow(Editor::IEditorWindow* window) = 0;

        virtual void ShowWindow(Editor::IEditorWindow* window) = 0;

        virtual void HideWindow(Editor::IEditorWindow* window) = 0;
    };
} // namespace Ame::Interfaces