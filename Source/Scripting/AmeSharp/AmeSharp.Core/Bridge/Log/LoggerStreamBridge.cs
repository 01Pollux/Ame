using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Bridge;
using AmeSharp.Core.Log;
using AmeSharp.Core.Log.Streams;
using AmeSharp.Core.Marshallers.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Bridge.Core.Log;

internal partial class LoggerStreamBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_Release")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void Release(IntPtr stream);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_SetPattern")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetPattern(IntPtr stream, NativeStringView pattern);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_SetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetLevel(IntPtr stream, LogLevel level);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_GetPattern")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    [return: MarshalUsing(typeof(NativeStringViewMarshaller))]
    public static partial NativeStringView GetPattern(IntPtr stream);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_GetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial LogLevel GetLevel(IntPtr stream);

    //

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateCallback")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateCallback();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_UpdateCallback")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void UpdateCallback(IntPtr stream, ICustomLoggerStream.LoggerStreamCallback callback, IntPtr userData);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateConsole")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateConsole();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateFile")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateFile(NativeStringView filePath, [MarshalAs(UnmanagedType.I1)] bool truncate);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateMsvcDebug")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateMsvcDebug();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateNull")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateNull();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_LoggerStream_CreateRotatingFile")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr CreateRotatingFile(NativeStringView baseFileName, ulong maxFileSize, uint maxBackupCount, [MarshalAs(UnmanagedType.I1)] bool rotateOnOpen);
}
