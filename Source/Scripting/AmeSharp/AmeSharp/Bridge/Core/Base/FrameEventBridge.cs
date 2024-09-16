using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class FrameEventBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_OnFrameStart_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr ConnectOnFrameStart(IntPtr frameEvent, IntPtr callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_GetOnFrameUpdate_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr ConnectOnFrameUpdate(IntPtr frameEvent, IntPtr callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_GetOnFrameEnd_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr ConnectOnFrameEnd(IntPtr frameEvent, IntPtr callback, IntPtr userData);
}
