#pragma once

#include <Core/Interface.hpp>
#include <EditorPlugin/EditorWindowManagerDesc.hpp>

namespace Ame
{
    // {0344717A-5B66-4242-9FE2-0D45AAA20674}
    inline constexpr UId IID_EditorModule{
        0x344717a, 0x5b66, 0x4242, { 0x9f, 0xe2, 0xd, 0x45, 0xaa, 0xa2, 0x6, 0x74 }
    };
    class EditorModule;

    // {BE08421C-CE71-4D12-8CDF-7BE552A8C033}
    inline constexpr UId IID_EditorWindowSubmodule{
        0xbe08421c, 0xce71, 0x4d12, { 0x8c, 0xdf, 0x7b, 0xe5, 0x52, 0xa8, 0xc0, 0x33 }
    };
    class EditorWindowSubmodule;

    struct EditorModuleConfig
    {
        Editor::EditorWindowManagerCreateDesc EditorManagerDesc;
    };
} // namespace Ame