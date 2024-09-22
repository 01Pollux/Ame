using AmeSharp.Rhi.Marshallers.Surface;
using System.Drawing;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Rhi.Surface;

public enum WindowSurfaceType : int
{
    Desktop
}

[NativeMarshalling(typeof(WindowCreateDescMarshaller))]
public record WindowCreateDesc
{
    public string Title = "";
    public Size Size = new(1024, 720);
    public bool CustomTitleBar = false;
    public bool StartInMiddle = true;
    public bool FullScreen = false;
    public bool Maximized = false;
    public bool NoResize = false;
}
