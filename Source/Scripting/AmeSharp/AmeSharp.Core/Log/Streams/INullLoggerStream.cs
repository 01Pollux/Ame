using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public sealed class INullLoggerStream : ILoggerStream
{
    private INullLoggerStream(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ILoggerStream Create() => new INullLoggerStream(LoggerStreamBridge.CreateMsvcDebug(), true);
}
