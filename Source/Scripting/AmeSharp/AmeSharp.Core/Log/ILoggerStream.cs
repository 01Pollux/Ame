using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Base;

namespace AmeSharp.Core.Log;

public abstract class ILoggerStream(IntPtr handle, bool ownsHandle) : INativeObject(handle, ownsHandle)
{
    public string Pattern
    {
        get => LoggerStreamBridge.GetPattern(Handle);
        set => LoggerStreamBridge.SetPattern(Handle, value);
    }

    public LogLevel Level
    {
        get => LoggerStreamBridge.GetLevel(Handle);
        set => LoggerStreamBridge.SetLevel(Handle, value);
    }

    protected override bool ReleaseHandle()
    {
        LoggerStreamBridge.Release(Handle);
        return true;
    }
}