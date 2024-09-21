using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class IRotatingFileLoggerStream(string baseFileName, ulong maxFileSize, uint maxBackupCount, bool rotateOnOpen) :
    ILoggerStream(LoggerStreamBridge.CreateRotatingFile(baseFileName, maxFileSize, maxBackupCount, rotateOnOpen))
{
}
