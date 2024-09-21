#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>
#include <Core/Ame.hpp>

namespace Ame::Editor
{
    // {97FB68DA-A89C-4938-A158-9402F5534427}
    static constexpr UId IID_EditorWindow{
        0x97fb68da, 0xa89c, 0x4938, { 0xa1, 0x58, 0x94, 0x2, 0xf5, 0x53, 0x44, 0x27 }
    };

    class IEditorWindow : public IObjectWithCallback
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_EditorWindow, IObjectWithCallback);

    public:
        IEditorWindow(IReferenceCounters* counters, String path) :
            IObjectWithCallback(counters), m_Path(std::move(path))
        {
        }

    public:
        /// <summary>
        /// Called when the window is visible and needs to be drawn.
        /// </summary>
        virtual void OnDrawVisible()
        {
        }

        /// <summary>
        /// Called to draw at the top of the window, toolbar.
        /// </summary>
        virtual void OnToolbarDraw()
        {
        }

        /// <summary>
        /// Called when the window is shown.
        /// </summary>
        virtual void OnShow()
        {
        }

        /// <summary>
        /// Called when the window is hidden.
        /// </summary>
        virtual void OnHide()
        {
        }

    public:
        [[nodiscard]] const String& GetFullPath() const noexcept
        {
            return m_Path;
        }

    private:
        /// <summary>
        /// Path is combined with window name and groups for toolbar. eg:
        /// "Scene View##General$Scene View" <- "Scene View" is the window name, "General" is the group, "Scene View" is
        /// the name inside tool bar.
        /// </summary>
        String m_Path;
    };
} // namespace Ame::Editor