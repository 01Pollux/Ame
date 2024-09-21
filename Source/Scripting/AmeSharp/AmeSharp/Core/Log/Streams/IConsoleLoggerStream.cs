using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class IConsoleLoggerStream() :
    ILoggerStream(LoggerStreamBridge.CreateConsole())
{
}
