#pragma once

#include <Core/Interface.hpp>

namespace Ame::Window
{
    // {2B4A9BAA-B9D0-4DEC-A84B-A0107392D2CD}
    inline constexpr UId IID_ImGuiWindow{ 0x2b4a9baa, 0xb9d0, 0x4dec, { 0xa8, 0x4b, 0xa0, 0x10, 0x73, 0x92, 0xd2, 0xcd } };

    /// <summary>
    /// Since GLFW is used for window creation, and it uses singleton for handling window events,
    /// Our only option is to delegate ImGui initialization to the window inside GLFW thread.
    /// This function also has no effect on release builds.
    /// The context must be set before calling these function.
    /// </summary>
    class IImGuiWindow : public IObject
    {
    public:
        /// <summary>
        /// Initialize ImGui for this window.
        /// </summary>
        virtual void InitializeImGui(
            void* imguiContext) = 0;

        /// <summary>
        /// Install ImGui callbacks for this window.
        /// </summary>
        virtual void InstallImGuiCallbacks(
            void* imguiContext) = 0;

        /// <summary>
        /// Initialize the new frame for ImGui for this window.
        /// </summary>
        virtual void NewFrameImGui(
            void* imguiContext) = 0;

        /// <summary>
        /// Shutdown ImGui for this window.
        /// </summary>
        virtual void UninstallImGuiCallbacks(
            void* imguiContext) = 0;

        /// <summary>
        /// Shutdown ImGui for this window.
        /// </summary>
        virtual void ShutdownImGui(
            void* imguiContext) = 0;
    };
} // namespace Ame::Window
