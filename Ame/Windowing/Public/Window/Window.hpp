#pragma once

#include <Object/Signal.hpp>

#include <Window/Desc.hpp>
#include <utility>

struct GLFWwindow;

namespace Ame::Windowing
{
    class Window
    {
    public:
        AME_SIGNAL_DECL(OnWindowSizeChanged, void(const Math::Size2I& newSize));
        AME_SIGNAL_DECL(OnWindowMinized, void(bool minimized));
        AME_SIGNAL_DECL(OnWindowClosed, void());
        AME_SIGNAL_DECL(OnWindowTitleHitTest, bool(const Math::Vector2I& mousePos));

    public:
        explicit Window(
            const WindowDesc& windowDesc);

        Window(
            Window&& other) noexcept :
            m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        Window& operator=(
            Window&& other) noexcept
        {
            if (this != &other)
            {
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

        ~Window();

    public:
        /// <summary>
        /// The handle of the window
        /// </summary>
        [[nodiscard]] GLFWwindow* GetHandle() const;

        /// <summary>
        /// Close the window
        /// The window will remain valid until its destructor is called
        /// </summary>
        void Close();

        /// <summary>
        /// Check whether the window is running or not
        /// </summary>
        [[nodiscard]] bool IsRunning() const;

        /// <summary>
        /// The title of the window
        /// </summary>
        [[nodiscard]] const String& GetTitle() const;

        /// <summary>
        /// Set the title of the window
        /// </summary>
        void SetTitle(
            String title);

        /// <summary>
        /// The position of the window in the screen
        /// </summary>
        [[nodiscard]] Math::Vector2I GetPosition() const;

        /// <summary>
        /// Set the position of the window
        /// </summary>
        void SetPosition(
            const Math::Vector2I& position);

        /// <summary>
        /// The size of the window
        /// </summary>
        [[nodiscard]] Math::Size2I GetSize() const;

        /// <summary>
        /// Set the size of the window
        /// </summary>
        void SetSize(
            const Math::Size2I& size);

        /// <summary>
        /// Set window to fullscreen
        /// </summary>
        void SetFullscreen(
            bool state);

        /// <summary>
        /// Whether the window is minimized or not
        /// </summary>
        [[nodiscard]] bool IsMinimized() const;

        /// <summary>
        /// Minimize the window
        /// </summary>
        void Minimize();

        /// <summary>
        /// Restore the window
        /// </summary>
        void Restore();

        /// <summary>
        /// Whether the window is maximized or not
        /// </summary>
        [[nodiscard]] bool IsMaximized() const;

        /// <summary>
        /// Maximize the window
        /// </summary>
        void Maximize();

        /// <summary>
        /// Whether the window is maximized or not
        /// </summary>
        [[nodiscard]] bool IsFullScreen() const;

        /// <summary>
        /// Whether the window is visible or not
        /// </summary>
        [[nodiscard]] bool IsVisible() const;

        /// <summary>
        /// Set window icon
        /// </summary>
        /// <param name="IconData">
        /// A pointer to the image data in RGBA format
        /// </param>
        /// <param name="Size">
        /// The size of the image
        /// </param>
        void SetIcon(
            const std::byte*      iconData,
            const Math::Vector2I& size);

        /// <summary>
        /// Set whether the window is visible or not
        /// </summary>
        void SetVisible(
            bool show);

        /// <summary>
        /// Set the current window to be made the active foreground window
        /// </summary>
        void RequestFocus();

        /// <summary>
        /// Whether the window has focus or not
        /// </summary>
        [[nodiscard]] bool HasFocus() const;

    public:
        /// <summary>
        /// Event fired when the window is resized
        /// </summary>
        AME_SIGNAL_INST(OnWindowSizeChanged);

        /// <summary>
        /// Event fired when the window is minimized or restored
        /// </summary>
        AME_SIGNAL_INST(OnWindowMinized);

        /// <summary>
        /// Event fired when the window is closed
        /// </summary>
        AME_SIGNAL_INST(OnWindowClosed);

        /// <summary>
        /// Event fired when the window title bar is hit
        /// </summary>
        AME_SIGNAL_INST(OnWindowTitleHitTest);

    public:
        /// <summary>
        /// Process events for the window
        /// This function should be called every frame
        /// Processes events such as input, resize, etc.
        /// </summary>
        void ProcessEvents();

    private:
        /// <summary>
        /// Create the glfw window
        /// This function must be called inside the glfw worker thread
        /// </summary>
        void CreateGlfwWindow(
            const WindowDesc& windowDesc);

    private:
        GLFWwindow*  m_Handle = nullptr;
        Math::Size2I m_WindowSize{};
        String       m_Title;
    };
} // namespace Ame::Windowing