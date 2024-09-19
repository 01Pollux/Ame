using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class MsvcDebugStream : ILoggerStream
{
    public MsvcDebugStream() : base(LoggerStreamBridge.CreateMsvcDebug())
    {
    }
}
