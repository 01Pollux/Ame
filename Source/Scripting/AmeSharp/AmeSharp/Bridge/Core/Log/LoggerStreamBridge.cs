using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Log;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Log;

internal partial class LoggerStreamBridge
{
    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Release(IntPtr streamObject);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_SetPattern")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetPattern(IntPtr streamObject, NativeStringView pattern);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_SetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetLevel(IntPtr streamObject, LogLevel level);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_GetPattern")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial NativeStringView GetPattern(IntPtr streamObject);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_GetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial LogLevel GetLevel(IntPtr streamObject);

    //

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateCallback")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe partial IntPtr CreateCallback(delegate* unmanaged[Cdecl]<NativeStringView, void> callback);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateConsole")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateConsole();

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateFile")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateFile(NativeStringView filePath, [MarshalAs(UnmanagedType.I1)] bool truncate);

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateMsvcDebug")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateMsvcDebug();

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateNull")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateNull();

    [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "Ame_LoggerStream_CreateRotatingFile")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateRotatingFile(NativeStringView baseFileName, ulong maxFileSize, uint maxBackupCount, [MarshalAs(UnmanagedType.I1)] bool rotateOnOpen);
}
