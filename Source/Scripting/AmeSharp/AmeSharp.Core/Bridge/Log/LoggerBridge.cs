using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Bridge;
using AmeSharp.Core.Log;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Bridge.Core.Log;

internal partial class LoggerBridge
{
    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_Create")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr Create(NativeStringView loggerName);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_SetInstance")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetInstance(IntPtr logger);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_GetInstance")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial IntPtr GetInstance();

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_AddStream")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void AddStream(IntPtr logger, ILoggerStream stream);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_RemoveStream")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void RemoveStream(IntPtr logger, ILoggerStream stream);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_WriteMessage")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void WriteMessage(IntPtr logger, LoggerInfo logInfo);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_GetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial LogLevel GetLevel(IntPtr logger);

    [LibraryImport(Libraries.AmeEngine, EntryPoint = "Ame_Logger_SetLevel")]
    [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
    public static partial void SetLevel(IntPtr logger, LogLevel level);
}
