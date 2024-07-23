#pragma once

#include <Module/Module.hpp>
#include <EditorPlugin/EditorWindowManagerDesc.hpp>

namespace Ame
{
    // {0344717A-5B66-4242-9FE2-0D45AAA20674}
    static const UId IID_EditorModule = { 0x344717a, 0x5b66, 0x4242, { 0x9f, 0xe2, 0xd, 0x45, 0xaa, 0xa2, 0x6, 0x74 } };

    struct EditorModuleConfig
    {
        Editor::EditorWindowManagerCreateDesc EditorManagerDesc;
    };

    class EditorWindowSubmodule;
} // namespace Ame