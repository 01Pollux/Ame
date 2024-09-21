using AmeSharp.Core.Bridge;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Base;

internal partial class SignalBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Signal_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Signal_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr signalObject);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Signal_Connect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr Connect(IntPtr signalObject, delegate* unmanaged[Cdecl]<IntPtr, IntPtr, void> callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Signal_Disconnect")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Disconnect(IntPtr signalConnection);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Signal_Invoke")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Emit(IntPtr signalObject, IntPtr args);
}
