using AmeSharp.Core.Base;
using AmeSharp.Rhi.Bridge.Surface;
using System.Drawing;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Surface;

[Guid("D04401D7-E3E3-45D8-B66D-FA5E05750D90")]
public class IWindow : IBaseObject
{
    public IWindow(IntPtr obj) : base(obj, true) { }
    public IWindow(WindowSurfaceType type, WindowCreateDesc createDesc) : base(WindowSurfaceBridge.Create(type, createDesc), true) { }

    public bool IsRunning => WindowSurfaceBridge.IsRunning(NativePointer);
    public void Close() => WindowSurfaceBridge.Close(NativePointer);

    public string Title
    {
        get => WindowSurfaceBridge.GetTitle(NativePointer);
        set => WindowSurfaceBridge.SetTitle(NativePointer, value);
    }

    public Size Size
    {
        get
        {
            int width, height;
            unsafe
            {
                WindowSurfaceBridge.GetSize(NativePointer, &width, &height);
            }
            return new(width, height);
        }
        set => WindowSurfaceBridge.SetSize(NativePointer, value.Width, value.Height);
    }

    public Point Position
    {
        get
        {
            int x, y;
            unsafe
            {
                WindowSurfaceBridge.GetPosition(NativePointer, &x, &y);
            }
            return new(x, y);
        }
        set => WindowSurfaceBridge.SetPosition(NativePointer, value.X, value.Y);
    }

    public bool FullScreen
    {
        get => WindowSurfaceBridge.IsFullScreen(NativePointer);
        set => WindowSurfaceBridge.SetFullscreen(NativePointer, value);
    }

    public bool Minimized => WindowSurfaceBridge.IsMinimized(NativePointer);
    public void Minimalize() => WindowSurfaceBridge.Minimize(NativePointer);

    public bool Maximized => WindowSurfaceBridge.IsMaximized(NativePointer);
    public void Maximize() => WindowSurfaceBridge.Maximize(NativePointer);

    public bool Visible
    {
        get => WindowSurfaceBridge.IsVisible(NativePointer);
        set => WindowSurfaceBridge.SetVisible(NativePointer, value);
    }

    public bool Focused => WindowSurfaceBridge.HasFocus(NativePointer);
    public void Focus() => WindowSurfaceBridge.RequestFocus(NativePointer);

    public void Restore() => WindowSurfaceBridge.Restore(NativePointer);
}

public class IDesktopWindow(WindowCreateDesc createDesc) : IWindow(WindowSurfaceType.Desktop, createDesc)
{
}