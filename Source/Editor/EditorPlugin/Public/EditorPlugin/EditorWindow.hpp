#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>
#include <Core/Ame.hpp>

namespace Ame::Editor
{
    class IEditorWindow : public IObject
    {
    public:
        IEditorWindow(String path) : m_Path(std::move(path))
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