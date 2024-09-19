using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class FileStream(string filePath, bool truncate = false) : ILoggerStream(LoggerStreamBridge.CreateFile(filePath, truncate))
{
}
