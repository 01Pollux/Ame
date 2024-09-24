using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public sealed class IFileLoggerStream : ILoggerStream
{
    private IFileLoggerStream(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ILoggerStream Create(string filePath, bool truncate = false) => new IFileLoggerStream(LoggerStreamBridge.CreateFile(filePath, truncate), true);
}
