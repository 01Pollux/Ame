using AmeSharp.Bridge.Core.Log;
using AmeSharp.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class IConsoleLoggerStream() :
    ILoggerStream(LoggerStreamBridge.CreateConsole())
{
}
