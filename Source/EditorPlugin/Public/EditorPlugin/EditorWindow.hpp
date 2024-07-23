#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>

namespace Ame::Editor
{
    // {306299DA-1AFC-406C-89F8-85F130ACB18E}
    static const UId IID_BaseEditorWindow = { 0x306299da, 0x1afc, 0x406c, { 0x89, 0xf8, 0x85, 0xf1, 0x30, 0xac, 0xb1, 0x8e } };

    // {4A0E2739-0BE5-499D-92D8-4CCDC7C375B5}
    static const UId             IID_ConsoleEditorWindow   = { 0x4a0e2739, 0xbe5, 0x499d, { 0x92, 0xd8, 0x4c, 0xcd, 0xc7, 0xc3, 0x75, 0xb5 } };
    static constexpr const char* c_ConsoleEditorWindowPath = "Console##General$Console";

    // {69473BB4-DF78-40BD-8352-0FFF945CAB08}
    static const UId             IID_ContentBrowserEditorWindow   = { 0x69473bb4, 0xdf78, 0x40bd, { 0x83, 0x52, 0xf, 0xff, 0x94, 0x5c, 0xab, 0x8 } };
    static constexpr const char* c_ContentBrowserEditorWindowPath = "Content Browser##General$Content Browser";

    // {1AD4023E-8139-4199-AC8B-C932796239FF}
    static const UId             IID_GameViewEditorWindow   = { 0x1ad4023e, 0x8139, 0x4199, { 0xac, 0x8b, 0xc9, 0x32, 0x79, 0x62, 0x39, 0xff } };
    static constexpr const char* c_GameViewEditorWindowPath = "Game View##General$Game View";

    // {E99E58F1-6AA9-4307-ADA3-9523E4A759E8}
    static const UId             IID_HierarchyEditorWindow   = { 0xe99e58f1, 0x6aa9, 0x4307, { 0xad, 0xa3, 0x95, 0x23, 0xe4, 0xa7, 0x59, 0xe8 } };
    static constexpr const char* c_HierarchyEditorWindowPath = "Hierarchy##General$Hierarchy";

    // {D742E1BA-CE72-4583-8178-72C18F887B1E}
    static const UId             IID_InspectorEditorWindow   = { 0xd742e1ba, 0xce72, 0x4583, { 0x81, 0x78, 0x72, 0xc1, 0x8f, 0x88, 0x7b, 0x1e } };
    static constexpr const char* c_InspectorEditorWindowPath = "Inspector##General$Inspector";

    // {A2346568-3B84-4B0F-A519-6967043B90A6}
    static const UId             IID_SceneViewEditorWindow   = { 0xa2346568, 0x3b84, 0x4b0f, { 0xa5, 0x19, 0x69, 0x67, 0x4, 0x3b, 0x90, 0xa6 } };
    static constexpr const char* c_SceneViewEditorWindowPath = "Scene View##General$Scene View";

    class IEditorWindow : public IObject
    {
    public:
        IEditorWindow(
            String path) :
            m_Path(std::move(path))
        {
        }

    public:
        /// <summary>
        /// Called when the window is visible and needs to be drawn.
        /// </summary>
        virtual void OnDrawVisible(){};

        /// <summary>
        /// Called to draw at the top of the window, toolbar.
        /// </summary>
        virtual void OnToolbarDraw(){};

        /// <summary>
        /// Called when the window is shown.
        /// </summary>
        virtual void OnShow(){};

        /// <summary>
        /// Called when the window is hidden.
        /// </summary>
        virtual void OnHide(){};

    public:
        [[nodiscard]] const String& GetFullPath() const
        {
            return m_Path;
        }

    private:
        /// <summary>
        /// Path is combined with window name and groups for toolbar. eg:
        /// "Scene View##General$Scene View" <- "Scene View" is the window name, "General" is the group, "Scene View" is the name inside tool bar.
        /// </summary>
        String m_Path;
    };
} // namespace Ame::Editor