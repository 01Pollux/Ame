#pragma once

#include <Core/UId.hpp>

namespace Ame::Editor
{
    // {4A0E2739-0BE5-499D-92D8-4CCDC7C375B5}
    inline constexpr UId IID_ConsoleEditorWindow{
        0x4a0e2739, 0xbe5, 0x499d, { 0x92, 0xd8, 0x4c, 0xcd, 0xc7, 0xc3, 0x75, 0xb5 }
    };
    inline constexpr const char* ConsoleEditorWindowPath = "Console##General$Console";

    // {69473BB4-DF78-40BD-8352-0FFF945CAB08}
    inline constexpr UId IID_ContentBrowserEditorWindow{
        0x69473bb4, 0xdf78, 0x40bd, { 0x83, 0x52, 0xf, 0xff, 0x94, 0x5c, 0xab, 0x8 }
    };
    inline constexpr const char* ContentBrowserEditorWindowPath = "Content Browser##General$Content Browser";

    // {1AD4023E-8139-4199-AC8B-C932796239FF}
    inline constexpr UId IID_GameViewEditorWindow{
        0x1ad4023e, 0x8139, 0x4199, { 0xac, 0x8b, 0xc9, 0x32, 0x79, 0x62, 0x39, 0xff }
    };
    inline constexpr const char* GameViewEditorWindowPath = "Game View##General$Game View";

    // {E99E58F1-6AA9-4307-ADA3-9523E4A759E8}
    inline constexpr UId IID_HierarchyEditorWindow{
        0xe99e58f1, 0x6aa9, 0x4307, { 0xad, 0xa3, 0x95, 0x23, 0xe4, 0xa7, 0x59, 0xe8 }
    };
    inline constexpr const char* HierarchyEditorWindowPath = "Hierarchy##General$Hierarchy";

    // {D742E1BA-CE72-4583-8178-72C18F887B1E}
    inline constexpr UId IID_InspectorEditorWindow{
        0xd742e1ba, 0xce72, 0x4583, { 0x81, 0x78, 0x72, 0xc1, 0x8f, 0x88, 0x7b, 0x1e }
    };
    inline constexpr const char* InspectorEditorWindowPath = "Inspector##General$Inspector";

    // {A2346568-3B84-4B0F-A519-6967043B90A6}
    inline constexpr UId IID_SceneViewEditorWindow{
        0xa2346568, 0x3b84, 0x4b0f, { 0xa5, 0x19, 0x69, 0x67, 0x4, 0x3b, 0x90, 0xa6 }
    };
    inline constexpr const char* SceneViewEditorWindowPath = "Scene View##General$Scene View";
} // namespace Ame::Editor