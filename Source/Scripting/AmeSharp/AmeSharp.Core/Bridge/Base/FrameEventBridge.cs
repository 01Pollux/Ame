using AmeSharp.Core.Base;
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
    public static unsafe partial IntPtr ConnectOnFrameStart(IFrameEvent frameEvent, VoidSignalConnection.SignalCallback callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_OnFrameUpdate_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr ConnectOnFrameUpdate(IFrameEvent frameEvent, VoidSignalConnection.SignalCallback callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_FrameEvent_OnFrameEnd_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr ConnectOnFrameEnd(IFrameEvent frameEvent, TypedSignalConnection<bool>.SignalCallback callback, IntPtr userData);
}
