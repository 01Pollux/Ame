using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log;

public class ILoggerStream(IntPtr nativePointer, ILoggerStream.LoggerStreamCallback? callback = null) : INativeObject(nativePointer)
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LoggerStreamCallback(NativeStringView message);
    private readonly LoggerStreamCallback? _loggerStreamCallback = callback;

    public string Pattern
    {
        get => LoggerStreamBridge.GetPattern(NativePointer);
        set => LoggerStreamBridge.SetPattern(NativePointer, value);
    }

    public LogLevel Level
    {
        get => LoggerStreamBridge.GetLevel(NativePointer);
        set => LoggerStreamBridge.SetLevel(NativePointer, value);
    }

    protected override void Dispose(bool disposed)
    {
        if (NativePointer != IntPtr.Zero)
        {
            LoggerStreamBridge.Release(NativePointer);
        }
        base.Dispose(disposed);
    }
}