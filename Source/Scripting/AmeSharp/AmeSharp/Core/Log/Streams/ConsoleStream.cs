using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class ConsoleStream : ILoggerStream
{
    public ConsoleStream() : base(LoggerStreamBridge.CreateConsole())
    {
    }
}
