using AmeSharp.Rhi.Surface;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Rhi.Marshallers.Surface;

[CustomMarshaller(typeof(WindowCreateDesc), MarshalMode.ManagedToUnmanagedIn, typeof(WindowCreateDescMarshaller))]
public static unsafe class WindowCreateDescMarshaller
{
    public struct Unmanaged
    {
        public byte* Title;
        public uint Width;
        public uint Height;
        public bool CustomTitleBar;
        public bool StartInMiddle;
        public bool FullScreen;
        public bool Maximized;
        public bool NoResize;
    }

    public static Unmanaged ConvertToUnmanaged(WindowCreateDesc managed)
    {
        return new()
        {
            Title = Utf8StringMarshaller.ConvertToUnmanaged(managed.Title),
            Width = (uint)managed.Size.Width,
            Height = (uint)managed.Size.Height,
            CustomTitleBar = managed.CustomTitleBar,
            StartInMiddle = managed.StartInMiddle,
            FullScreen = managed.FullScreen,
            Maximized = managed.Maximized,
            NoResize = managed.NoResize
        };
    }

    public static void Free(Unmanaged unmanaged)
    {
        Utf8StringMarshaller.Free(unmanaged.Title);
    }
}
