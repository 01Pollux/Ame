using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public sealed class IConsoleLoggerStream : ILoggerStream
{
    private IConsoleLoggerStream(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ILoggerStream Create() => new IConsoleLoggerStream(LoggerStreamBridge.CreateConsole(), true);
}
