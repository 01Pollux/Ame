using AmeSharp.Core.Base;
using AmeSharp.Rhi.Bridge.Surface;
using System.Drawing;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Surface;

[Guid("D04401D7-E3E3-45D8-B66D-FA5E05750D90")]
public class IWindow : IBaseObject
{
    protected IWindow(WindowSurfaceType type, WindowCreateDesc createDesc) : base(WindowSurfaceBridge.Create(type, createDesc), true, false) { }
    public static IWindow Create(WindowSurfaceType type, WindowCreateDesc createDesc) => new(type, createDesc);

    public bool IsRunning => WindowSurfaceBridge.IsRunning(this);
    public void CloseWindow() => WindowSurfaceBridge.Close(this);

    public string Title
    {
        get => WindowSurfaceBridge.GetTitle(this);
        set => WindowSurfaceBridge.SetTitle(this, value);
    }

    public Size Size
    {
        get
        {
            int width, height;
            unsafe
            {
                WindowSurfaceBridge.GetSize(this, &width, &height);
            }
            return new(width, height);
        }
        set => WindowSurfaceBridge.SetSize(this, value.Width, value.Height);
    }

    public Point Position
    {
        get
        {
            int x, y;
            unsafe
            {
                WindowSurfaceBridge.GetPosition(this, &x, &y);
            }
            return new(x, y);
        }
        set => WindowSurfaceBridge.SetPosition(this, value.X, value.Y);
    }

    public bool FullScreen
    {
        get => WindowSurfaceBridge.IsFullScreen(this);
        set => WindowSurfaceBridge.SetFullscreen(this, value);
    }

    public bool Minimized => WindowSurfaceBridge.IsMinimized(this);
    public void Minimalize() => WindowSurfaceBridge.Minimize(this);

    public bool Maximized => WindowSurfaceBridge.IsMaximized(this);
    public void Maximize() => WindowSurfaceBridge.Maximize(this);

    public bool Visible
    {
        get => WindowSurfaceBridge.IsVisible(this);
        set => WindowSurfaceBridge.SetVisible(this, value);
    }

    public bool Focused => WindowSurfaceBridge.HasFocus(this);
    public void Focus() => WindowSurfaceBridge.RequestFocus(this);

    public void Restore() => WindowSurfaceBridge.Restore(this);
}

public class IDesktopWindow : IWindow
{
    internal IDesktopWindow(WindowCreateDesc createDesc) : base(WindowSurfaceType.Desktop, createDesc) { }
    public static IDesktopWindow Create(WindowCreateDesc createDesc) => new(createDesc);
}