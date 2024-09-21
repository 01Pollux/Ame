using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class IMsvcDebugLoggerStream : ILoggerStream
{
    public IMsvcDebugLoggerStream() : base(LoggerStreamBridge.CreateMsvcDebug())
    {
    }
}
