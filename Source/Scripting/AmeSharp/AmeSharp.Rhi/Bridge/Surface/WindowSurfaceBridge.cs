using AmeSharp.Core.Base.Types;
using AmeSharp.Rhi.Surface;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Rhi.Bridge.Surface;

internal partial class WindowSurfaceBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create(WindowSurfaceType type, WindowCreateDesc createDesc);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_IsRunning")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsRunning(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_Close")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Close(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_SetTitle")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetTitle(IWindow window, NativeStringView title);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_SetSize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetSize(IWindow window, int width, int height);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_SetPosition")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetPosition(IWindow window, int x, int y);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_GetTitle")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial NativeStringView GetTitle(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_GetSize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void GetSize(IWindow window, int* width, int* height);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_GetPosition")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial void GetPosition(IWindow window, int* x, int* y);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_IsFullScreen")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsFullScreen(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_SetFullscreen")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetFullscreen(IWindow window, [MarshalAs(UnmanagedType.I1)] bool state);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_IsMinimized")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsMinimized(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_IsMaximized")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsMaximized(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_Maximize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Maximize(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_Minimize")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Minimize(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_IsVisible")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool IsVisible(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_SetVisible")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetVisible(IWindow window, [MarshalAs(UnmanagedType.I1)] bool show);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_HasFocus")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool HasFocus(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_RequestFocus")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void RequestFocus(IWindow window);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_IWindow_Restore")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Restore(IWindow window);
}
