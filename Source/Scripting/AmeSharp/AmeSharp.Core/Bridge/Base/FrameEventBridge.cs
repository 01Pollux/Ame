using AmeSharp.Core.Bridge;
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
    public static unsafe partial IntPtr ConnectOnFrameStart(IntPtr frameEvent, delegate* unmanaged[Cdecl]<IntPtr, IntPtr, void> callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_OnFrameUpdate_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr ConnectOnFrameUpdate(IntPtr frameEvent, delegate* unmanaged[Cdecl]<IntPtr, IntPtr, void> callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_OnFrameEnd_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr ConnectOnFrameEnd(IntPtr frameEvent, delegate* unmanaged[Cdecl]<IntPtr, IntPtr, bool> callback, IntPtr userData);
}
