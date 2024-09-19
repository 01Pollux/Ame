using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class NullStream : ILoggerStream
{
    public NullStream() : base(LoggerStreamBridge.CreateNull())
    {
    }
}
