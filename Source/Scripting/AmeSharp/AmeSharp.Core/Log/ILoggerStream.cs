using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;
using AmeSharp.Core.Base.Types;

namespace AmeSharp.Core.Log;

public class ILoggerStream(nint obj) : INativeObject(obj)
{
    public delegate void LoggerStreamCallback(NativeStringView message);

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
        if (NativePointer != nint.Zero)
        {
            LoggerStreamBridge.Release(NativePointer);
        }
        base.Dispose(disposed);
    }
}