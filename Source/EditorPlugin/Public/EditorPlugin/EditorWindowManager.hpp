#pragma once

#include <Core/Interface.hpp>
#include <Core/Export.hpp>

namespace Ame::Editor
{
    struct EditorWindowManagerCreateDesc;
    struct EditorWindowCreateDesc;
    class IEditorWindow;

    // {0150C09D-7439-4D7B-A6C0-A398004E3E8F}
    static const UId IID_EditorWindowManager = { 0x150c09d, 0x7439, 0x4d7b, { 0xa6, 0xc0, 0xa3, 0x98, 0x0, 0x4e, 0x3e, 0x8f } };

    class IEditorWindowManager : public IObject
    {
    public:
        virtual void ResetDefaultWindows() = 0;

        virtual void AddWindow(
            const EditorWindowCreateDesc& desc) = 0;

        virtual void RemoveWindow(
            IEditorWindow* window) = 0;

        virtual void ShowWindow(
            IEditorWindow* window) = 0;

        virtual void HideWindow(
            IEditorWindow* window) = 0;
    };

    [[nodiscard]] AME_API Ptr<IEditorWindowManager> CreateEditorWindowManager(
        const EditorWindowManagerCreateDesc& desc);
} // namespace Ame::Editor