using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public sealed class IRotatingFileLoggerStream : ILoggerStream
{
    private IRotatingFileLoggerStream(IntPtr handle, bool ownsHandle) : base(handle, ownsHandle) { }
    public static ILoggerStream Create(string baseFileName, ulong maxFileSize, uint maxBackupCount, bool rotateOnOpen) =>
        new IRotatingFileLoggerStream(LoggerStreamBridge.CreateRotatingFile(baseFileName, maxFileSize, maxBackupCount, rotateOnOpen), true);
}
