using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public sealed class IMsvcDebugLoggerStream : ILoggerStream
{
    private IMsvcDebugLoggerStream(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ILoggerStream Create() => new IMsvcDebugLoggerStream(LoggerStreamBridge.CreateMsvcDebug(), true);
}
